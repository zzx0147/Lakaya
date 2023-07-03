#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
	// 현재 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// A팀인 상태 Anti Lakaya
	A UMETA(DisplayerName = "A"),

	// B팀인 상태 Pro Lakaya
	B UMETA(DisplayerName = "B"),

	// 개인전
	Individual UMETA(DisplayerName = "Individual")
};

inline bool JudgeSameTeam(const EPlayerTeam& First, const EPlayerTeam& Second)
{
	// 팀이 없거나 개인전 상태가 아니고, 두 팀의 값이 같다면 같은 팀으로 판정합니다.
	return First != EPlayerTeam::None && First != EPlayerTeam::Individual && First == Second;
}