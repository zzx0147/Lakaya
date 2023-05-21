#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
	// 현재 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// A팀인 상태
	A UMETA(DisplayerName = "A"),

	// B팀인 상태
	B UMETA(DisplayerName = "B"),

	// 개인전
	Individual UMETA(DisplayerName = "Individual")
};