#define DO_CHECK 1

#include "UI/SkillWidget.h"

#include <filesystem>

#include "Character/ArmedCharacter.h"
#include "UI/SkillProgressBar.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QSkillProgressBar = Cast<USkillProgressBar>(GetWidgetFromName("Skill_Q"));
	ESkillProgressBar = Cast<USkillProgressBar>(GetWidgetFromName("Skill_E"));
	RButtonSkillProgressBar = Cast<USkillProgressBar>(GetWidgetFromName("Skill_RButton"));

	check(QSkillProgressBar != nullptr);
	check(ESkillProgressBar != nullptr);
	check(RButtonSkillProgressBar != nullptr);
}

void USkillWidget::SetCharacter(const FName& CharacterName)
{
	if(SkillWidgetTextureMap.Contains(CharacterName))
	{
		QSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].QSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].QSkillTextureFillImage);
		ESkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].ESkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].ESkillTextureFillImage);
		RButtonSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].RButtonSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].RButtonSkillTextureFillImage);
	}
}

USkillProgressBar* USkillWidget::GetSkillProgressBar(const EAbilityKind AbilityKind)
{
	switch (AbilityKind)
	{
	case Primary:
		return QSkillProgressBar.Get();
	case Secondary:
		return ESkillProgressBar.Get();
	case WeaponAbility:
		return RButtonSkillProgressBar.Get();
	}
	
	return nullptr;
}
