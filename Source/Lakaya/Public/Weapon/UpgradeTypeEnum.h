#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class UpgradeTypeEnum : uint8
{
	Bullet,
	Damage,
	Range,
	SkillRange,
	SkillDamage,
};
