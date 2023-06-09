#define DO_CHECK 1

#include "UI/SkillWidget.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RenaQSkill = Cast<UImage>(GetWidgetFromName("Rena_Skill_Q"));
	RenaESkill = Cast<UImage>(GetWidgetFromName("Rena_Skill_E"));
	RenaRButtonSkill = Cast<UImage>(GetWidgetFromName("Rena_Skill_RButton"));
	// RenaRSkill = Cast<UImage>(GetWidgetFromName("Rena_Skill_R"));

	WaziQSkill = Cast<UImage>(GetWidgetFromName("Wazi_Skill_Q"));
	WaziESkill = Cast<UImage>(GetWidgetFromName("Wazi_Skill_E"));
	WaziRButtonSkill = Cast<UImage>(GetWidgetFromName("Wazi_Skill_RButton"));
	// WaziRSkill = Cast<UImage>(GetWidgetFromName("Wazi_Skill_R"));

	check(RenaQSkill != nullptr);
	check(RenaESkill != nullptr);
	check(RenaRButtonSkill != nullptr);
	// check(RenaRSkill != nullptr);

	check(WaziQSkill != nullptr);
	check(WaziESkill != nullptr);
	check(WaziRButtonSkill != nullptr);
	// check(WaziRSkill != nullptr);
}