#include "UI/SkillWidget.h"
#include "Character/ArmedCharacter.h"
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

USkillProgressBar* USkillWidget::GetSkillProgressBar(const ESkillKey& SkillKey) const
{
	switch (SkillKey)
	{
	case ESkillKey::Q:
		return QSkillProgressBar.Get();
	case ESkillKey::E:
		return ESkillProgressBar.Get();
	case ESkillKey::RMB:
		return RMBSkillProgressBar.Get();
	default:
		return nullptr;
	}
}
