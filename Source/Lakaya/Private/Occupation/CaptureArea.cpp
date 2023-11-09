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

	CaptureAreaWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
}

void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();

	OccupyExpressElementWidget = Cast<UOccupyExpressElementWidget>(CaptureAreaWidgetComponent->GetWidget());
	if (OccupyExpressElementWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupyExpressElementWidget is null."));
		return;
	}

	OccupyExpressElementWidget->GetProgressBar()->SetPercent(0);

	switch (CaptureAreaId)
	{
	case 1:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetAntiAreaNoneImage());
		break;
	case 2:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetCenterAreaNoneImage());
		break;
	case 3:
		OccupyExpressElementWidget->GetInImage()->SetBrushFromTexture(OccupyExpressElementWidget->GetProAreaNoneImage());
		break;
	default: ;
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
		const auto* OverlappedArmedCharacter = Cast<ACharacter>(OtherActor);

		if (OverlappedArmedCharacter)
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
		const auto* OverlappedArmedCharacter = Cast<ACharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			if (auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 충돌이 끝난 액터가 캐릭터입니다.
				RemoveFromOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);

				// AimOccupyProgressWidget을 0으로 초기화해줍니다.
				if (OccupyingPlayerState && OccupyingPlayerState->GetPawn())
				{
					if (const auto PlayerController = OccupyingPlayerState->GetPawn()->IsLocallyControlled())
					{
						if (OccupyingPlayerState->GetAimOccupyProgressWidget())
						{
							OccupyingPlayerState->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(0, false);
							OccupyingPlayerState->GetAimOccupyProgressWidget()->AllAimWidgetDisable();
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
	
	switch (CaptureAreaId)
	{
	case 1:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetAntiAreaAntiImage() :
																&OccupyExpressElementWidget->GetAntiAreaProImage();
		break;
	case 2:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetCenterAreaAntiImage() :
			&OccupyExpressElementWidget->GetCenterAreaProImage();
		break;
	case 3:
		WidgetImage = (CurrentCaptureAreaTeam == ETeam::Anti) ? &OccupyExpressElementWidget->GetProAreaAntiImage() :
			&OccupyExpressElementWidget->GetProAreaProImage();
		break;
		default:
		UE_LOG(LogTemp, Warning, TEXT("존재하지 않는 점령 ID 입니다."));
		break;
	}

	const TObjectPtr<UMaterialInterface> TeamMaterial = (NewTeam == ETeam::Anti) ? AntiMaterial : ProMaterial;
	StaticMeshComponent->SetMaterial(0, TeamMaterial);
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

		for (const auto& Team : {ETeam::Anti, ETeam::Pro})
		{
			for (const auto& Player : OccupyingPlayerList[Team])
			{
				if (const auto PlayerController = Player->GetPawn()->IsLocallyControlled())
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
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::AntiProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::IncreaseCaptureProgress,
		                                       0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::ProExtortion)
	{
		SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀의 점령구역입니다."));
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress,
		                                       0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::Anti)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구 입니다."));
	}
}

// Anti팀 0명, Pro팀 1명이상
void ACaptureArea::CaptureAreaHandleProTeam(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::None || CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState
		== ECaptureAreaState::Opposite
		|| CaptureAreaState == ECaptureAreaState::ProExtortion || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::ProProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::IncreaseCaptureProgress,
		                                       0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::AntiExtortion)
	{
		SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀의 점령구역입니다."));
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress,
		                                       0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구역 입니다."));
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
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀과 Pro팀이 서로 대치하고 있는 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::Opposite);
			break;
		case ETeam::Anti:
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 Anti팀의 점령구역을 탈취하려는 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::ProExtortion);
			break;
		case ETeam::Pro:
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 Pro팀의 점령구역을 탈취하려는 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::AntiExtortion);
			break;
		default:
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Switch case Error."));
			break;
		}

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Anti)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 Anti팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::ProExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 Pro팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::AntiExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

// Anti팀 0명, Pro팀 0명
void ACaptureArea::CaptureAreaHandleNone(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState == ECaptureAreaState::ProExtortion)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro || CaptureAreaState == ECaptureAreaState::AntiExtortion)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
	}
	else if (CaptureAreaState == ECaptureAreaState::ProProgress || CaptureAreaState == ECaptureAreaState::AntiProgress)
	{
		if (GetCurrentCaptureAreaTeam() == ETeam::None) // 어느팀에서도 점령을 하지 않은 상태라면
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 None 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::None);
		}
		else if (GetCurrentCaptureAreaTeam() == ETeam::Anti)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
		}
		else if (GetCurrentCaptureAreaTeam() == ETeam::Pro)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
		}
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::DecreaseCaptureProgress,
	                                       0.1f, true);
}

void ACaptureArea::IncreaseCaptureProgress()
{
	const ECaptureAreaState CaptureAreaState = GetCurrentCaptureAreaState();

	if (CaptureAreaState == ECaptureAreaState::AntiProgress || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		const ETeam CurrentTeam = (CaptureAreaState == ECaptureAreaState::AntiProgress) ? ETeam::Anti : ETeam::Pro;
		float& TeamCaptureProgress = (CurrentTeam == ETeam::Anti) ? AntiTeamCaptureProgress : ProTeamCaptureProgress;
		TeamCaptureProgress += CaptureSpeed * 0.1f;
		
		const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
		if (OccupationGameState == nullptr)
		{
			return;
		}

		for (const auto& Player : OccupyingPlayerList[CurrentTeam])
		{
			if (const auto PlayerController = Player->GetPawn()->IsLocallyControlled())
				Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(TeamCaptureProgress, true);
			UE_LOG(LogTemp, Warning, TEXT("true"));
		}

		OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);

		UE_LOG(LogTemp, Warning, TEXT("%f"), TeamCaptureProgress);

		if (CurrentTeam == ETeam::Anti)
		{
			AntiTeamCaptureProgress = TeamCaptureProgress;
			OccupyExpressElementWidget->GetProgressBar()->SetFillImage(OccupyExpressElementWidget->GetAntiFillImage());
			
		}
		else if (CurrentTeam == ETeam::Pro)
		{
			ProTeamCaptureProgress = TeamCaptureProgress;
			OccupyExpressElementWidget->GetProgressBar()->SetFillImage(OccupyExpressElementWidget->GetProFillImage());
		}

		OccupyExpressElementWidget->GetProgressBar()->SetPercent(TeamCaptureProgress / 4.0f);
		
		// 점령에 성공했다면
		if (TeamCaptureProgress >= 4.0f)
		{
			const ETeam OtherTeam = (CurrentTeam == ETeam::Anti) ? ETeam::Pro : ETeam::Anti;
			if (GetCurrentCaptureAreaTeam() == OtherTeam)
			{
				OccupationGameState->SubCaptureAreaCount(OtherTeam);
			}

			OccupationGameState->AddCaptureAreaCount(CurrentTeam, CaptureAreaId);
			OccupationGameState->SetCaptureOwnerChange(CaptureAreaId, CurrentTeam);
			SetCurrentCaptureAreaState((CurrentTeam == ETeam::Anti) ? ECaptureAreaState::Anti : ECaptureAreaState::Pro);
			TeamCaptureProgress = 0.0f;
			SetCurrentCaptureAreaTeam(CurrentTeam);
			CaptureAreaTeamOnChangedSignature.Broadcast(GetCurrentCaptureAreaTeam());
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);

			OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);

			for (const auto& Player : OccupyingPlayerList[CurrentTeam])
			{
				if (const auto PlayerController = Player->GetPawn()->IsLocallyControlled())
				{
					Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(TeamCaptureProgress, false);
					Player->GetAimOccupyProgressWidget()->Success();
				}
			}

			if (OccupationGameState->CheckCaptureAreaCount(CurrentTeam))
			{
				OccupationGameState->SetTeamToUpdate(CurrentTeam);
				OccupationGameState->StartScoreUpdate(CurrentTeam, 1.0f);
			}
		}
	}
}

void ACaptureArea::DecreaseCaptureProgress()
{
	AntiTeamCaptureProgress -= CaptureSpeed * 0.1f;
	ProTeamCaptureProgress -= CaptureSpeed * 0.1f;
	UE_LOG(LogTemp, Warning, TEXT("Anti : %f"), AntiTeamCaptureProgress);
	UE_LOG(LogTemp, Warning, TEXT("Pro : %f"), ProTeamCaptureProgress);

	const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (OccupationGameState == nullptr)
	{
		return;
	}

	const float TeamCaptureProgress = AntiTeamCaptureProgress > ProTeamCaptureProgress
		                                  ? AntiTeamCaptureProgress
		                                  : ProTeamCaptureProgress;
	const ETeam CurrentTeam = AntiTeamCaptureProgress > ProTeamCaptureProgress ? ETeam::Anti : ETeam::Pro;
	OccupyExpressElementWidget->GetProgressBar()->SetPercent(TeamCaptureProgress / 4.0f);
	for (const auto& Player : OccupyingPlayerList[CurrentTeam])
	{
		if (const auto PlayerController = Player->GetPawn()->IsLocallyControlled())
			Player->GetAimOccupyProgressWidget()->SetAimOccupyProgressBar(TeamCaptureProgress, false);
	}

	OccupationGameState->UpdateExpressWidget(CurrentTeam, CaptureAreaId, TeamCaptureProgress);

	if (AntiTeamCaptureProgress <= 0.0f && ProTeamCaptureProgress <= 0.0f)
	{
		AntiTeamCaptureProgress = 0.0f;
		ProTeamCaptureProgress = 0.0f;

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

FString ACaptureArea::GetEnumAsString(const ECaptureAreaState& EnumValue)
{
	const TObjectPtr<UEnum> EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/Lakaya.ECaptureAreaState"));
	if (EnumPtr)
	{
		return EnumPtr->GetNameStringByIndex(static_cast<uint8>(EnumValue));
	}

	return FString("Casting Failed.");
}

FString ACaptureArea::ETeamToString(const ETeam& Team)
{
	switch (Team)
	{
	case ETeam::Anti:
		return TEXT("Anti");
	case ETeam::Pro:
		return TEXT("Pro");
	case ETeam::None:
		return TEXT("None");
	default:
		return TEXT("Unknown");
	}
}

