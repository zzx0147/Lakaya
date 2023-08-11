// Fill out your copyright notice in the Description page of Project Settings.

#include "Occupation/CaptureArea.h"

#include "MovieSceneCaptureSettings.h"
#include "Character/ArmedCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/BoxComponent.h"
#include "GameMode/OccupationGameState.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProfilingDebugging/BootProfiling.h"

ACaptureArea::ACaptureArea()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	CaptureAreaRange = FVector(1000.0f, 1000.0f, 1000.0f);
	
	RootComponent = Trigger;

	Trigger->SetBoxExtent(CaptureAreaRange);
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArea::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACaptureArea::OnOverlapEnd);

	OccupyingPlayerList.Emplace(ETeam::Anti);
	OccupyingPlayerList.Emplace(ETeam::Pro);

	AntiTeamCaptureProgress = 0.0f;
	ProTeamCaptureProgress = 0.0f;
	CaptureSpeed = 1.0f;
}

void ACaptureArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureArea, CurrentCaptureAreaState);
	DOREPLIFETIME(ACaptureArea, CurrentCaptureAreaTeam);
}

void ACaptureArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌한 액터가 캐릭터인지 확인합니다.
		const AArmedCharacter* OverlappedArmedCharacter = Cast<AArmedCharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			if(auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 겹친 액터가 캐릭터입니다.
				AddToOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Character Cast Failed."));	
			}
		}
		else
		{
			// 겹친 액터는 캐릭터가 아닙니다.
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlapped by non-ArmedCharacter"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Same Trigger"));
	}
}

void ACaptureArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 동일한 액터를 확인하여 self-overlaps를 무시합니다.
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// 충돌이 끝난 액터가 캐릭터인지 확인합니다.
		const AArmedCharacter* OverlappedArmedCharacter = Cast<AArmedCharacter>(OtherActor);

		if (OverlappedArmedCharacter)
		{
			if(auto OccupyingPlayerState = Cast<ALakayaBasePlayerState>(OverlappedArmedCharacter->GetPlayerState()))
			{
				// 충돌이 끝난 액터가 캐릭터입니다.
				RemoveFromOccupyPlayerList(OccupyingPlayerState->GetTeam(), OccupyingPlayerState);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Cast Failed."));	
			}
		}
		else
		{
			// 겹친 액터는 캐릭터가 아닙니다.
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Trigger Overlapped by non-ArmedCharacter"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Same Trigger"));
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

void ACaptureArea::UpdateCaptureAreaState(const ECaptureAreaState& CaptureAreaState)
{
	const uint8 AntiTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Anti) ? OccupyingPlayerList[ETeam::Anti].Num() : 0;
	const uint8 ProTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Pro) ? OccupyingPlayerList[ETeam::Pro].Num() : 0;
	
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
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount == 0)
	{
		CaptureAreaHandleNone(CaptureAreaState);
	}

	CaptureAreaStateOnChangedSignature.Broadcast(CurrentCaptureAreaState);
	const FString EnumString = GetEnumAsString(CurrentCaptureAreaState);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, EnumString);
}

// Anti팀 1명이상, Pro팀 0명
void ACaptureArea::CaptureAreaHandleAntiTeam(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::None || CaptureAreaState == ECaptureAreaState::Pro || CaptureAreaState == ECaptureAreaState::Opposite
		|| CaptureAreaState == ECaptureAreaState::AntiExtortion || CaptureAreaState == ECaptureAreaState::AntiProgress)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::AntiProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::UpdateCaptureProgress, 0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::ProExtortion)
	{
		SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀의 점령구역입니다."));
	}
	else if (CaptureAreaState == ECaptureAreaState::Anti)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구 입니다."));
	}
}

// Anti팀 0명, Pro팀 1명이상
void ACaptureArea::CaptureAreaHandleProTeam(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::None || CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState == ECaptureAreaState::Opposite
		|| CaptureAreaState == ECaptureAreaState::ProExtortion || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::ProProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArea::UpdateCaptureProgress, 0.1f, true);
	}
	else if (CaptureAreaState == ECaptureAreaState::AntiExtortion)
	{
		SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀의 점령구역입니다."));
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구역 입니다."));
	}
}

// Anti팀 1명이상, Pro팀 1명이상
void ACaptureArea::CaptureAreaHandleOpposite(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::AntiProgress || CaptureAreaState == ECaptureAreaState::ProProgress)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀과 Pro팀이 서로 대치하고 있는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::Opposite);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Anti)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 Anti팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::ProExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 Pro팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::AntiExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

// Anti팀 0명, Pro팀 0명
void ACaptureArea::CaptureAreaHandleNone(const ECaptureAreaState& CaptureAreaState)
{
	if (CaptureAreaState == ECaptureAreaState::Anti || CaptureAreaState == ECaptureAreaState::ProExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::Anti);

		// TODO: Pro팀이 점령도중 죽게 되었을 때 로직 추가
		
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::Pro || CaptureAreaState == ECaptureAreaState::AntiExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
		SetCurrentCaptureAreaState(ECaptureAreaState::Pro);

		// TODO: Anti팀이 점령도중 죽게 되었을 때 로직 추가

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureAreaState == ECaptureAreaState::ProProgress || CaptureAreaState == ECaptureAreaState::AntiProgress)
	{
		if (GetCurrentCaptureAreaTeam() == ETeam::None) // 어느팀에서도 점령을 하지 않은 상태라면
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 None 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::None);
		}
		else if (GetCurrentCaptureAreaTeam() == ETeam::Anti)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
		}
		else if (GetCurrentCaptureAreaTeam() == ETeam::Pro)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
			SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
		}

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

void ACaptureArea::UpdateCaptureProgress()
{
	if (GetCurrentCaptureAreaState() == ECaptureAreaState::AntiProgress)
	{
		AntiTeamCaptureProgress += CaptureSpeed * 0.1f;

		if (AntiTeamCaptureProgress >= 4.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("AntiTeam Capture Success."));
			const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
			if (OccupationGameState == nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("OccupationGameState is null."));
			}

			if (GetCurrentCaptureAreaTeam() == ETeam::Pro)
			{
				OccupationGameState->SubCaptureAreaCount(ETeam::Pro);
			}

			OccupationGameState->AddCaptureAreaCount(ETeam::Anti);
			SetCurrentCaptureAreaState(ECaptureAreaState::Anti);
			AntiTeamCaptureProgress = 0.0f;
			SetCurrentCaptureAreaTeam(ETeam::Anti);
			CaptureAreaTeamOnChangedSignature.Broadcast(GetCurrentCaptureAreaTeam());
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
			
			if (bool bIncrease = OccupationGameState->CheckCaptureAreaCount(ETeam::Anti))
			{
				OccupationGameState->SetTeamToUpdate(ETeam::Anti);
				OccupationGameState->StartScoreUpdate(ETeam::Anti, 1.0f);
			}
		}
	}
	else if (GetCurrentCaptureAreaState() == ECaptureAreaState::ProProgress)
	{
		ProTeamCaptureProgress += CaptureSpeed * 0.1f;

		if (ProTeamCaptureProgress >= 4.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("ProTream Capture Success."));
			const auto OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
			if (OccupationGameState == nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("OccupationGameState is null."));
			}
			
			if (GetCurrentCaptureAreaTeam() == ETeam::Anti)
			{
				OccupationGameState->SubCaptureAreaCount(ETeam::Anti);
			}

			OccupationGameState->AddCaptureAreaCount(ETeam::Pro);
			SetCurrentCaptureAreaState(ECaptureAreaState::Pro);
			ProTeamCaptureProgress = 0.0f;
			SetCurrentCaptureAreaTeam(ETeam::Pro);
			CaptureAreaTeamOnChangedSignature.Broadcast(GetCurrentCaptureAreaTeam());
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
			
			if (bool bIncrease = OccupationGameState->CheckCaptureAreaCount(ETeam::Pro))
			{
				OccupationGameState->SetTeamToUpdate(ETeam::Pro);
				OccupationGameState->StartScoreUpdate(ETeam::Pro, 1.0f);
			}
		}
	}
}

FString ACaptureArea::GetEnumAsString(const ECaptureAreaState& EnumValue)
{
	const TObjectPtr<UEnum> EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/Lakaya.ECaptureAreaState"));
	if (EnumPtr)
	{
		return EnumPtr->GetNameStringByIndex(static_cast<uint8>(EnumValue));
	}
	else
	{
		return FString("Casting Failed.");
	}
}