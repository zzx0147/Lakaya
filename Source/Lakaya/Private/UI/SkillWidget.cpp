#define DO_CHECK 1

#include "UI/SkillWidget.h"

#include <filesystem>

#include "Character/ArmedCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "UI/SkillProgressBar.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void USkillWidget::SetCharacter(const FName& CharacterName)
{
	if(SkillWidgetTextureMap.Contains(CharacterName))
	{
		QSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].QSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].QSkillTextureFillImage);
		ESkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].ESkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].ESkillTextureFillImage);
		RMBSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].RMBSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].RMBSkillTextureFillImage);
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
		return RMBSkillProgressBar.Get();
	}
	
	return nullptr;
}
