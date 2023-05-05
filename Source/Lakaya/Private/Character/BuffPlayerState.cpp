// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BuffPlayerState.h"

#include "Character/LakayaBaseCharacter.h"
#include "Net/UnrealNetwork.h"

void ABuffPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuffPlayerState, KillStreakBuffLevel);
}

void ABuffPlayerState::IncreaseKillStreak()
{
	Super::IncreaseKillStreak();

	// 캐릭터로부터 연속처치시 적용될 버프 정보를 가져와서 버프를 적용합니다.
	if (const auto Character = GetPawn<ALakayaBaseCharacter>())
		if (auto& Buffs = Character->GetKillStreakBuffs(); Buffs.IsValidIndex(KillStreakBuffLevel))
		{
			ApplyBuff(Buffs[KillStreakBuffLevel]);
			SetKillStreakBuffLevel(KillStreakBuffLevel + 1);
		}
}

void ABuffPlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	Super::OnPawnSetCallback(Player, NewPawn, OldPawn);

	if (const auto Character = Cast<ALakayaBaseCharacter>(OldPawn); Character && KillStreakBuffLevel != 0)
	{
		auto& Buffs = Character->GetKillStreakBuffs();

		// 연속처치 시스템에 의해 적용되었던 버프들을 즉시 제거합니다.
		for (int16 Index = KillStreakBuffLevel; Index >= 0; --Index)
			if (Buffs.IsValidIndex(Index))
				ExpireBuff(Buffs[Index]);

		SetKillStreakBuffLevel(0);
	}
}

void ABuffPlayerState::ApplyBuff(const FName& BuffName)
{
	//TODO: 데이터테이블에서 버프를 찾고 적용합니다.
}

void ABuffPlayerState::ExpireBuff(const FName& BuffName)
{
	//TODO: 버프 목록에서 버프를 찾아 적용된 버프를 제거합니다.
}

void ABuffPlayerState::OnRep_KillStreakBuffLevel()
{
	OnKillStreakBuffLevelChanged.Broadcast(KillStreakBuffLevel);
}

void ABuffPlayerState::SetKillStreakBuffLevel(const uint8& NewLevel)
{
	KillStreakBuffLevel = NewLevel;
	OnKillStreakBuffLevelChanged.Broadcast(KillStreakBuffLevel);
}
