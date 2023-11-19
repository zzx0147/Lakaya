// Fill out your copyright notice in the Description page of Project Settings.

#include "Occupation/CaptureArea.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/Character.h"
#include "GameMode/OccupationGameState.h"
#include "Net/UnrealNetwork.h"
#include "UI/AimOccupyProgressWidget.h"

ACaptureArea::ACaptureArea()
{
	PrimaryActorTick.bCanEverTick = false;

	OccupyingPlayerList.Emplace(ETeam::Anti);
	OccupyingPlayerList.Emplace(ETeam::Pro);

	AntiTeamCaptureProgress = 0.0f;
	ProTeamCaptureProgress = 0.0f;
	CaptureSpeed = 1.0f;
	CaptureAreaId = 100;
	MaterialValue = 0.5f;
	InterpSpeed = 5.0f;
	SuccessPlayer = nullptr;

	CaptureAreaWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
}

void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();

	OccupyExpressElementWidget = Cast<UOccupyExpressElementWidget>(CaptureAreaWidgetComponent->GetWidget());
	if (!IsValid(OccupyExpressElementWidget) || !IsValid(OccupyExpressElementWidget->GetInImage())) return;
	
	DynamicMaterial = StaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, NeutralMaterial);
	if (IsValid(DynamicMaterial))
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("0=NonOccupation_1=Occupation"), 0);
		DynamicMaterial->SetScalarParameterValue(TEXT("0=Anti_1=Pro"), 0);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("DynamicMaterial is null."));
	
	switch (CaptureAreaId)
	{
	case 1:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetFloatingAntiAreaNoneImage());
		break;
	case 2:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetFloatingCenterAreaNoneImage());
		break;
	case 3:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetFloatingProAreaNoneImage());
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("점령 ID가 존재하지 않습니다. 점령 ID를 부여해주세요."));
		break;
	}
}

void ACaptureArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureArea, CurrentCaptureAreaState);
	DOREPLIFETIME(ACaptureArea, CurrentCaptureAreaTeam);
}

void ACaptureArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌한 액터가 캐릭터인지 확인합니다.
		if (const auto OverlappedArmedCharacter = Cast<ACharacter>(OtherActor))
		{
			if (const auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// OccupyPlayerList에 추가합니다.
				AddToOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);
			}
		}
	}
}

void ACaptureArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌이 끝난 액터가 캐릭터인지 확인합니다.

		if (const auto OverlappedArmedCharacter = Cast<ACharacter>(OtherActor))
		{
			if (const auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 충돌이 끝난 액터가 캐릭터입니다.
				RemoveFromOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);

				// AimOccupyProgressWidget을 0으로 초기화해줍니다.
				if (IsValid(OccupyingPlayerState) && IsValid(OccupyingPlayerState->GetPawn()))
				{
					if (OccupyingPlayerState->GetPawn()->IsLocallyControlled())
					{
						if (IsValid(OccupyingPlayerState->GetAimOccupyProgressWidget()))
						{
							OccupyingPlayerState->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(OccupyingPlayerState->GetTeam() ,0, false);
							OccupyingPlayerState->GetAimOccupyProgressWidget()->InitAimOccupyWidget();
						}
					}
				}
			}
		}
	}
}

void ACaptureArea::AddToOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(PlayerTeam))
	{
		OccupyingPlayerList[PlayerTeam].Add(Player);
	}

	UpdateCaptureAreaState(CurrentCaptureAreaState);
}

void ACaptureArea::RemoveFromOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(PlayerTeam))
	{
		OccupyingPlayerList[PlayerTeam].Remove(Player);
	}
	
	UpdateCaptureAreaState(CurrentCaptureAreaState);
}

void ACaptureArea::SetCurrentCaptureAreaTeam(const ETeam& NewTeam)
{
	CurrentCaptureAreaTeam = NewTeam;
	const TObjectPtr<UTexture2D>* WidgetImage = nullptr;
	
	if(!IsValid(OccupyExpressElementWidget)) return;
	
	switch (CaptureAreaId)
	{
	case 1:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetFloatingAntiAreaAntiImage() :
																&OccupyExpressElementWidget->GetFloatingAntiAreaProImage();
		break;
	case 2:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetFloatingCenterAreaAntiImage():
																&OccupyExpressElementWidget->GetFloatingCenterAreaProImage();
		break;
	case 3:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetFloatingProAreaAntiImage() :
																&OccupyExpressElementWidget->GetFloatingProAreaProImage();
		break;
		default:
		UE_LOG(LogTemp, Warning, TEXT("점령 ID가 존재하지 않습니다. 점령 ID를 부여해주세요."));
		break;
	}

	// TODO : None 상태에서 Pro팀이 점령 했을 시, 잠시 녹색으로 보였다가 빨간색으로 바뀜. 후에 확인해봐야 합니다.
	if(IsValid(DynamicMaterial)) DynamicMaterial->SetScalarParameterValue(TEXT("0=NonOccupation_1=Occupation"), 1.0f);
	
	OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(*WidgetImage);
	OccupyExpressElementWidget->GetProgressBar()->SetPercent(0);
}

void ACaptureArea::UpdateCaptureAreaState(const ECaptureAreaState& CaptureAreaState)
{
	const uint8 AntiTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Anti)
		                                  ? OccupyingPlayerList[ETeam::Anti].Num()
		                                  : 0;
	const uint8 ProTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Pro)
		                                 ? OccupyingPlayerList[ETeam::Pro].Num()
		                                 : 0;

	if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount == 0)
	{
		CaptureAreaHandleAntiTeam(CaptureAreaState);
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount > 0)
	{
		CaptureAreaHandleProTeam(CaptureAreaState);
	}
	else if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount > 0)
	{
		CaptureAreaHandleOpposite(CaptureAreaState);

		for (const auto Team : {ETeam::Anti, ETeam::Pro})
		{
			for (const auto Player : OccupyingPlayerList[Team])
			{
				if (!IsValid(Player) || !IsValid(Player->GetPawn()) || !IsValid(Player->GetAimOccupyProgressWidget())) return;
				if (Player->GetPawn()->IsLocallyControlled())
					Player->GetAimOccupyProgressWidget()->OccupyCrash();
			}
		}
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount == 0)
	{
		CaptureAreaHandleNone(CaptureAreaState);
	}

	CaptureAreaStateOnChangedSignature.Broadcast(CurrentCaptureAreaState);
}

// Anti팀 1명이상, Pro팀 0명
void ACaptureArea::CaptureAreaHandleAntiTeam(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::None || CaptureAreaState == ECaptureAreaState::Pro || CaptureAreaState ==
		ECaptureAreaState::Opposite
		|| CaptureAreaState == ECaptureAreaState::AntiExtortion || CaptureAreaState == ECaptureAreaState::AntiProgress)
	{
		CurrentCaptureAreaState = ECaptureAreaState::AntiProgress;
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::IncreaseCaptureProgress, 0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::ProExtortion)
	{
		CurrentCaptureAreaState = ECaptureAreaState::Anti;
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress, 0.1f, true);
	}
}

// Anti팀 0명, Pro팀 1명이상
void ACaptureArea::CaptureAreaHandleProTeam(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::None || CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState
		== ECaptureAreaState::Opposite
		|| CaptureAreaState == ECaptureAreaState::ProExtortion || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		CurrentCaptureAreaState = ECaptureAreaState::ProProgress;
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::IncreaseCaptureProgress, 0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::AntiExtortion)
	{
		CurrentCaptureAreaState = ECaptureAreaState::Pro;
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress, 0.1f, true);
	}
}

// Anti팀 1명이상, Pro팀 1명이상
void ACaptureArea::CaptureAreaHandleOpposite(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::AntiProgress || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		switch (CurrentCaptureAreaTeam)
		{
		case ETeam::None:
			CurrentCaptureAreaState = ECaptureAreaState::Opposite;
			break;
		case ETeam::Anti:
			CurrentCaptureAreaState = ECaptureAreaState::ProExtortion;
			break;
		case ETeam::Pro:
			CurrentCaptureAreaState = ECaptureAreaState::AntiExtortion;
			break;
		default:
			break;
		}

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Anti)
	{
		CurrentCaptureAreaState = ECaptureAreaState::ProExtortion;
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro)
	{
		CurrentCaptureAreaState = ECaptureAreaState::AntiExtortion;
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

// Anti팀 0명, Pro팀 0명
void ACaptureArea::CaptureAreaHandleNone(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState == ECaptureAreaState::ProExtortion)
		CurrentCaptureAreaState = ECaptureAreaState::Anti;
	else if (CaptureAreaState == ECaptureAreaState::Pro || CaptureAreaState == ECaptureAreaState::AntiExtortion)
		CurrentCaptureAreaState = ECaptureAreaState::Pro;
	else if (CaptureAreaState == ECaptureAreaState::ProProgress || CaptureAreaState == ECaptureAreaState::AntiProgress)
	{
		if (CurrentCaptureAreaTeam == ETeam::None)
			CurrentCaptureAreaState = ECaptureAreaState::None;
		else if (CurrentCaptureAreaTeam == ETeam::Anti)
			CurrentCaptureAreaState = ECaptureAreaState::Anti;
		else if (CurrentCaptureAreaTeam == ETeam::Pro)
			CurrentCaptureAreaState = ECaptureAreaState::Pro;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress, 0.1f, true);
}

void ACaptureArea::IncreaseCaptureProgress()
{
	const ECaptureAreaState CaptureAreaState = CurrentCaptureAreaState;

	if(!IsValid(this)) return;
	
	if (CaptureAreaState == ECaptureAreaState::AntiProgress || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		const ETeam CurrentTeam = (CaptureAreaState == ECaptureAreaState::AntiProgress) ? ETeam::Anti : ETeam::Pro;
		float& TeamCaptureProgress = (CurrentTeam == ETeam::Anti) ? AntiTeamCaptureProgress : ProTeamCaptureProgress;
		TeamCaptureProgress += CaptureSpeed * 0.1f;
		
		const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
		if (!IsValid(OccupationGameState)) return;

		for (const auto Player : OccupyingPlayerList[CurrentTeam])
		{

			if(!IsValid(Player)) return;
			if(!IsValid(Player->GetPawn())) return;
			
			if (Player->GetPawn()->IsLocallyControlled() && IsValid(Player->GetAimOccupyProgressWidget()))
				Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(Player->GetTeam() ,TeamCaptureProgress, true);
		}

		OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);
		
		if (CurrentTeam == ETeam::Anti)
		{
			AntiTeamCaptureProgress = TeamCaptureProgress;
			if(IsValid(OccupyExpressElementWidget) && IsValid(OccupyExpressElementWidget->GetProgressBar()))
				OccupyExpressElementWidget->GetProgressBar()->SetFillImage(OccupyExpressElementWidget->GetFloatingAntiFillImage());
		}
		else if (CurrentTeam == ETeam::Pro)
		{
			ProTeamCaptureProgress = TeamCaptureProgress;
			if(IsValid(OccupyExpressElementWidget) && IsValid(OccupyExpressElementWidget->GetProgressBar()))
				OccupyExpressElementWidget->GetProgressBar()->SetFillImage(OccupyExpressElementWidget->GetFloatingProFillImage());
		}
		
		if(IsValid(OccupyExpressElementWidget) && IsValid(OccupyExpressElementWidget->GetProgressBar()))
			OccupyExpressElementWidget->GetProgressBar()->SetPercent(TeamCaptureProgress / 4.0f);

		// Capture Success.
		if (TeamCaptureProgress >= 4.0f)
		{
			if (const ETeam OtherTeam = (CurrentTeam == ETeam::Anti) ? ETeam::Pro : ETeam::Anti; CurrentCaptureAreaTeam == OtherTeam)
				OccupationGameState->SubCaptureAreaCount(OtherTeam);

			OccupationGameState->AddCaptureAreaCount(CurrentTeam, CaptureAreaId);
			OccupationGameState->SetCaptureOwnerChange(CaptureAreaId, CurrentTeam);
			CurrentCaptureAreaState = (CurrentTeam == ETeam::Anti) ? ECaptureAreaState::Anti : ECaptureAreaState::Pro;
			TeamCaptureProgress = 0.0f;
			SetCurrentCaptureAreaTeam(CurrentTeam);
			CaptureAreaTeamOnChangedSignature.Broadcast(CurrentCaptureAreaTeam);
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

			OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);

			// 점령구역에 제일 먼저 들어와 있던 인원에게 점수를 부여합니다.
			for (const auto Player : OccupyingPlayerList[CurrentTeam])
			{
				if (!IsValid(Player)) return;
				if (!IsValid(Player->GetPawn())) return;
				
				if (Player == OccupyingPlayerList[CurrentTeam][0])
				{
					Player->AddTotalScoreCount(500);
					Player->IncreaseSuccessCaptureCount();
					Player->IncreaseCurrentCaptureCount();

					if (SuccessPlayer != nullptr)
					{
						SuccessPlayer->DecreaseCurrentCaptureCount();
						SuccessPlayer = Player;
					}
					else if (SuccessPlayer == nullptr)
					{
						SuccessPlayer = Player;
					}
				}


				if (Player->GetPawn()->IsLocallyControlled())
				{
					if (IsValid(Player->GetAimOccupyProgressWidget()) && IsValid(Player->GetAimOccupyProgressWidget()))
					{
						Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(Player->GetTeam() ,TeamCaptureProgress, false);
						Player->GetAimOccupyProgressWidget()->OccupySuccess();
					}
				}
			}

			// if (OccupationGameState->CheckCaptureAreaCount(CurrentTeam))
			// {
			// 	OccupationGameState->SetTeamToUpdate(CurrentTeam);
			// 	OccupationGameState->StartScoreUpdate(CurrentTeam, 1.0f);
			// }

			OccupationGameState->CheckCaptureAreaCount(CurrentTeam);
		}

		GetWorld()->GetTimerManager().SetTimer(MaterialUpdateTimerHandle, this, &ACaptureArea::UpdateMaterialValue, 0.1f, true);
	}
}

void ACaptureArea::DecreaseCaptureProgress()
{
	AntiTeamCaptureProgress -= CaptureSpeed * 0.1f;
	ProTeamCaptureProgress -= CaptureSpeed * 0.1f;

	const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (!IsValid(OccupationGameState)) return;

	const float TeamCaptureProgress = AntiTeamCaptureProgress > ProTeamCaptureProgress ? AntiTeamCaptureProgress : ProTeamCaptureProgress;
	const ETeam CurrentTeam = AntiTeamCaptureProgress > ProTeamCaptureProgress ? ETeam::Anti : ETeam::Pro;
	
	if(IsValid(OccupyExpressElementWidget) && IsValid(OccupyExpressElementWidget->GetProgressBar()))
		OccupyExpressElementWidget->GetProgressBar()->SetPercent(TeamCaptureProgress / 4.0f);
	
	for (const auto Player : OccupyingPlayerList[CurrentTeam])
	{
		if(!IsValid(Player)) return;
		if(!IsValid(Player->GetPawn())) return;
		
		if (Player->GetPawn()->IsLocallyControlled() && IsValid(Player->GetAimOccupyProgressWidget()))
			Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(Player->GetTeam() ,TeamCaptureProgress, false);
	}

	OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);

	if (AntiTeamCaptureProgress <= 0.0f && ProTeamCaptureProgress <= 0.0f)
	{
		AntiTeamCaptureProgress = 0.0f;
		ProTeamCaptureProgress = 0.0f;

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

void ACaptureArea::UpdateMaterialValue()
{
	if (CurrentCaptureAreaTeam == ETeam::None) return;

	const float TargetValue = (CurrentCaptureAreaTeam == ETeam::Anti) ? 0.0f : 1.0f;
	if (CurrentCaptureAreaTeam == ETeam::Anti && MaterialValue <= 0.1f)
	{
		MaterialValue = 0;
		GetWorld()->GetTimerManager().ClearTimer(MaterialUpdateTimerHandle);
	}
	else if (CurrentCaptureAreaTeam == ETeam::Pro && MaterialValue >= 0.9f)
	{
		MaterialValue = 1.0f;
		GetWorld()->GetTimerManager().ClearTimer(MaterialUpdateTimerHandle);
	}

	MaterialValue = FMath::FInterpTo(MaterialValue, TargetValue, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	if (IsValid(DynamicMaterial))
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("0=Anti_1=Pro"), MaterialValue);
	}
}