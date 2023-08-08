// Fill out your copyright notice in the Description page of Project Settings.

#include "Occupation/CaptureArena.h"

#include "Character/ArmedCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/BoxComponent.h"
#include "GameMode/OccupationGameState.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProfilingDebugging/BootProfiling.h"

ACaptureArena::ACaptureArena()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	CaptureAreaRange = FVector(1000.0f, 1000.0f, 1000.0f);
	
	RootComponent = Trigger;

	Trigger->SetBoxExtent(CaptureAreaRange);
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArena::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACaptureArena::OnOverlapEnd);

	OccupyingPlayerList.Emplace(ETeam::Anti);
	OccupyingPlayerList.Emplace(ETeam::Pro);

	AntiTeamCaptureProgress = 0.0f;
	ProTeamCaptureProgress = 0.0f;
	CaptureSpeed = 1.0f;
}

void ACaptureArena::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureArena, CurrentCaptureArenaState);
	DOREPLIFETIME(ACaptureArena, CurrentCaptureArenaTeam);
}

void ACaptureArena::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void ACaptureArena::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void ACaptureArena::AddToOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(PlayerTeam))
	{
		OccupyingPlayerList[PlayerTeam].Add(Player);
	}

	CheckCaptureArenaInPlayer(PlayerTeam);
}

void ACaptureArena::RemoveFromOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player)
{
	if (OccupyingPlayerList.Contains(PlayerTeam))
	{
		OccupyingPlayerList[PlayerTeam].Remove(Player);
	}

	CheckCaptureArenaInPlayer(PlayerTeam);
}

void ACaptureArena::CheckCaptureArenaInPlayer(const ETeam& PlayerTeam)
{
	UE_LOG(LogTemp, Warning, TEXT("AntiTeam Occupying Player Number : %d"), OccupyingPlayerList[ETeam::Anti].Num());
	UE_LOG(LogTemp, Warning, TEXT("ProTeam Occupying Player Number : %d"), OccupyingPlayerList[ETeam::Pro].Num());

	const uint8 AntiTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Anti) ? OccupyingPlayerList[ETeam::Anti].Num() : 0;
	const uint8 ProTeamPlayerCount = OccupyingPlayerList.Contains(ETeam::Pro) ? OccupyingPlayerList[ETeam::Pro].Num() : 0;

	UpdateCaptureArenaState(AntiTeamPlayerCount, ProTeamPlayerCount, CurrentCaptureArenaState);
}

void ACaptureArena::UpdateCaptureArenaState(const uint8& AntiTeamPlayerCount, const uint8& ProTeamPlayerCount, const ECaptureArenaState& CaptureArenaState)
{
	if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount == 0)
	{
		CaptureArenaHandleAntiTeam(CaptureArenaState);
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount > 0)
	{
		CaptureArenaHandleProTeam(CaptureArenaState);
	}
	else if (AntiTeamPlayerCount > 0 && ProTeamPlayerCount > 0)
	{
		CaptureArenaHandleOpposite(CaptureArenaState);
	}
	else if (AntiTeamPlayerCount == 0 && ProTeamPlayerCount == 0)
	{
		CaptureArenaHandleNone(CaptureArenaState);
	}

	CaptureArenaStateOnChangedSignature.Broadcast(CurrentCaptureArenaState);
	const FString EnumString = GetEnumAsString(CurrentCaptureArenaState);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, EnumString);
}

// Anti팀 1명이상, Pro팀 0명
void ACaptureArena::CaptureArenaHandleAntiTeam(const ECaptureArenaState& CaptureArenaState)
{
	if (CaptureArenaState == ECaptureArenaState::None || CaptureArenaState == ECaptureArenaState::Pro || CaptureArenaState == ECaptureArenaState::Opposite
		|| CaptureArenaState == ECaptureArenaState::AntiExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::AntiProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArena::UpdateCaptureProgress, 0.1f, true);
	}
	else if (CaptureArenaState == ECaptureArenaState::Anti)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구 입니다."));
	}
}

// Anti팀 0명, Pro팀 1명이상
void ACaptureArena::CaptureArenaHandleProTeam(const ECaptureArenaState& CaptureArenaState)
{
	if (CaptureArenaState == ECaptureArenaState::None || CaptureArenaState == ECaptureArenaState::Anti || CaptureArenaState == ECaptureArenaState::Opposite
		|| CaptureArenaState == ECaptureArenaState::ProExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 점령을 시도하고 있는 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::ProProgress);
		if (GetWorld()->GetTimerManager().IsTimerActive(CaptureProgressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(CaptureProgressTimerHandle, this, &ACaptureArena::UpdateCaptureProgress, 0.1f, true);
	}
	else if (CaptureArenaState == ECaptureArenaState::Pro)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("이미 본인 소유의 점령구역 입니다."));
	}
}

// Anti팀 1명이상, Pro팀 1명이상
void ACaptureArena::CaptureArenaHandleOpposite(const ECaptureArenaState& CaptureArenaState)
{
	if (CaptureArenaState == ECaptureArenaState::AntiProgress || CaptureArenaState == ECaptureArenaState::ProProgress)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀과 Pro팀이 서로 대치하고 있는 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::Opposite);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureArenaState == ECaptureArenaState::Anti)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Pro팀이 Anti팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::ProExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureArenaState == ECaptureArenaState::Pro)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Anti팀이 Pro팀의 점령구역을 탈취하려는 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::AntiExtortion);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

// Anti팀 0명, Pro팀 0명
void ACaptureArena::CaptureArenaHandleNone(const ECaptureArenaState& CaptureArenaState)
{
	if (CaptureArenaState == ECaptureArenaState::Anti || CaptureArenaState == ECaptureArenaState::ProExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::Anti);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureArenaState == ECaptureArenaState::Pro || CaptureArenaState == ECaptureArenaState::AntiExtortion)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
		SetCurrentCaptureArenaState(ECaptureArenaState::Pro);
		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
	else if (CaptureArenaState == ECaptureArenaState::ProProgress || CaptureArenaState == ECaptureArenaState::AntiProgress)
	{
		if (GetCurrentCaptureArenaTeam() == ETeam::None) // 어느팀에서도 점령을 하지 않은 상태라면
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 None 상태입니다."));
			SetCurrentCaptureArenaState(ECaptureArenaState::None);
		}
		else if (GetCurrentCaptureArenaTeam() == ETeam::Anti)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Anti 상태입니다."));
		}
		else if (GetCurrentCaptureArenaTeam() == ETeam::Pro)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("점령구역이 Pro 상태입니다."));
		}

		GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
	}
}

void ACaptureArena::UpdateCaptureProgress()
{
	if (GetCurrentCaptureArenaState() == ECaptureArenaState::AntiProgress)
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

			if (GetCurrentCaptureArenaTeam() == ETeam::Pro)
			{
				OccupationGameState->SubCaptureAreaCount(ETeam::Pro);
			}

			OccupationGameState->AddCaptureAreaCount(ETeam::Anti);
			SetCurrentCaptureArenaState(ECaptureArenaState::Anti);
			AntiTeamCaptureProgress = 0.0f;
			SetCurrentCaptureArenaTeam(ETeam::Anti);
			CaptureArenaTeamOnChangedSignature.Broadcast(GetCurrentCaptureArenaTeam());
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
			
			if (bool bIncrease = OccupationGameState->CheckCaptureAreaCount(ETeam::Anti))
			{
				OccupationGameState->SetTeamToUpdate(ETeam::Anti);
				OccupationGameState->StartScoreUpdate(ETeam::Anti, 1.0f);
			}
		}
	}
	else if (GetCurrentCaptureArenaState() == ECaptureArenaState::ProProgress)
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
			
			if (GetCurrentCaptureArenaTeam() == ETeam::Anti)
			{
				OccupationGameState->SubCaptureAreaCount(ETeam::Anti);
			}

			OccupationGameState->AddCaptureAreaCount(ETeam::Pro);
			SetCurrentCaptureArenaState(ECaptureArenaState::Pro);
			ProTeamCaptureProgress = 0.0f;
			SetCurrentCaptureArenaTeam(ETeam::Pro);
			CaptureArenaTeamOnChangedSignature.Broadcast(GetCurrentCaptureArenaTeam());
			GetWorld()->GetTimerManager().ClearTimer(CaptureProgressTimerHandle);
			
			if (bool bIncrease = OccupationGameState->CheckCaptureAreaCount(ETeam::Pro))
			{
				OccupationGameState->SetTeamToUpdate(ETeam::Pro);
				OccupationGameState->StartScoreUpdate(ETeam::Pro, 1.0f);
			}
		}
	}
}

FString ACaptureArena::GetEnumAsString(const ECaptureArenaState& EnumValue)
{
	const TObjectPtr<UEnum> EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/Lakaya.ECaptureArenaState"));
	if (EnumPtr)
	{
		return EnumPtr->GetNameStringByIndex(static_cast<uint8>(EnumValue));
	}
	else
	{
		return FString("Casting Failed.");
	}
}
