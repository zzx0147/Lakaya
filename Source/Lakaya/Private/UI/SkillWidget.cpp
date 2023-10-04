#include "UI/SkillWidget.h"
#include "UI/SkillProgressBar.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void USkillWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (QSkillProgressBar) QSkillProgressBar->SetTexture(SkillWidgetData.QSkillTextureBackgroundImage, SkillWidgetData.QSkillTextureFillImage);
	if (ESkillProgressBar) ESkillProgressBar->SetTexture(SkillWidgetData.ESkillTextureBackgroundImage, SkillWidgetData.ESkillTextureFillImage);
	if (RMBSkillProgressBar) RMBSkillProgressBar->SetTexture(SkillWidgetData.RMBSkillTextureBackgroundImage, SkillWidgetData.RMBSkillTextureFillImage);

	if (QSkillProgressBar)
	{
		QSkillProgressBar->SetProgressType(SkillWidgetData.QSkillType);
		QSkillProgressBar->SetKey(ESkillKey::Q);
		QSkillProgressBar->SetTag(SkillWidgetData.QSkillTag);
	}
	
	if (ESkillProgressBar) 
	{
		ESkillProgressBar->SetProgressType(SkillWidgetData.ESkillType);
		ESkillProgressBar->SetKey(ESkillKey::E);
		ESkillProgressBar->SetTag(SkillWidgetData.ESkillTag);
	}
	
	if (RMBSkillProgressBar) 
	{
		RMBSkillProgressBar->SetProgressType(SkillWidgetData.RMBSkillType);
		RMBSkillProgressBar->SetKey(ESkillKey::RMB);
		RMBSkillProgressBar->SetTag(SkillWidgetData.RMBSkillTag);
	}
	
}

void USkillWidget::SetCharacter(const FName& CharacterName)
{
	// if(SkillWidgetTextureMap.Contains(CharacterName))
	// {
	// 	QSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].QSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].QSkillTextureFillImage);
	// 	ESkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].ESkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].ESkillTextureFillImage);
	// 	RMBSkillProgressBar->SetTexture(SkillWidgetTextureMap[CharacterName].RMBSkillTextureBackgroundImage,SkillWidgetTextureMap[CharacterName].RMBSkillTextureFillImage);
	// }
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

TArray<USkillProgressBar*> USkillWidget::GetAllSkillProgressBar()
{
	return {QSkillProgressBar, ESkillProgressBar, RMBSkillProgressBar};
}
