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
	case Q:
		return QSkillProgressBar.Get();
	case E:
		return ESkillProgressBar.Get();
	case RMB:
		return RMBSkillProgressBar.Get();
	default:
		return nullptr;
	}
}
