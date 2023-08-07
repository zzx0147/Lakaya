#pragma once

#include "CoreMinimal.h"
#include "Team.generated.h"

template <typename TEnum1, typename TEnum2>
bool CompareEnums(TEnum1 enumValue1, TEnum2 enumValue2)
{
	return static_cast<uint8>(enumValue1) == static_cast<uint8>(enumValue2);
}

UENUM(BlueprintType)
enum class ETeam : uint8
{
	// 현재 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// Anti팀에 배정된 상태
	Anti UMETA(DisplayerName = "Anti"),

	// Pro팀에 배정된 상태
	Pro UMETA(DisplayerName = "Pro"),

	// 개인전
	Individual UMETA(DisplayerName = "Individual")
};

UENUM()
enum class ECaptureArenaState : uint8
{
	// 아무도 점령하지 않은 상태입니다.
	None UMETA(DisplayerName = "None"),

	// Anti이 점령을 시도하고 있는 상태입니다.
	AntiProgress UMETA(DisplayerName = "AntiProgress"),

	// Anti이 Pro팀 점령구역을 탈취하려는 상태입니다.
	AntiExtortion UMETA(DisplayerName = "AntiExtortion"),
	
	// Anti팀이 점령한 상태입니다.
	Anti UMETA(DisplayerName = "Anti"),

	// Pro팀에서 점령을 시도하는 상태입니다.
	ProProgress UMETA(DisplayerName = "ProProgress"),

	// Pro팀이 Anti팀 점령구역을 탈취하려는 상태입니다..
	ProExtortion UMETA(DisplayerName = "ProExtortion"),
	
	// Pro팀에서 점령한 상태입니다.
	Pro UMETA(DisplayerName = "Pro"),

	// Anti님과 Pro팀이 점령되지 않은 점령구역에서 대치하고 있는 상태입니다.
	Opposite UMETA(DisPlayerName = "Opposite"),
};

inline bool JudgeSameTeam(const ETeam& First, const ETeam& Second)
{
	// 팀이 없거나 개인전 상태가 아니고, 두 팀의 값이 같다면 같은 팀으로 판정합니다.
	return First != ETeam::None && First != ETeam::Individual && First == Second;
}

/**
 * @brief 점령구역의 소속팀과, 플레이어가 소속된 팀을 비교하는 함수입니다.
 * @param PlayerTeam 점령구역에 있는 플레이어의 팀 정보입니다.
 * @param CaptureArenaState 점령구역의 소속팀입니다.
 * @return 같다면 true를 반환하고, 다르다면 false를 반환합니다.
 */
inline bool JudgeSameTeam(const ETeam& PlayerTeam, const ECaptureArenaState& CaptureArenaState)
{
	if (PlayerTeam != ETeam::None && PlayerTeam != ETeam::Individual)
	{
		if (CompareEnums(PlayerTeam, CaptureArenaState))
		{
			return true;
		}
	}
	else
	{
		return false;
	}

	return false;
}