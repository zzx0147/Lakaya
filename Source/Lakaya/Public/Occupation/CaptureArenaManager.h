#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "CaptureArenaManager.generated.h"

UENUM()
enum class ECaptureArenaState : uint8
{
	// 아무도 점령하지 않은 상태입니다.
	None UMETA(DisplayerName = "None"),

	// Anti팀에서 점령을 시도하는 상태입니다.
	AntiProgress UMETA(DisplayerName = "AntiProgress"),

	// Anti팀이 점령한 상태입니다.
	Anti UMETA(DisplayerName = "Anti"),

	// Pro팀에서 점령을 시도하는 상태입니다.
	ProProgress UMETA(DisplayerName = "ProProgress"),

	// Pro팀에서 점령한 상태입니다.
	Pro UMETA(DisplayerName = "Pro"),

	// Anti님과 Pro팀이 대치하고 있는 상태입니다.
	Opposite UMETA(DisPlayerName = "Opposite"),
};

// inline bool JudgeSameTeam(const ECaptureArenaState& State, const EPlayerTeam& Player)
// {
	// return 
// }