#include "UI/SkillWidget.h"
#include "UI/SkillProgressBar.h"

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

void USkillWidget::SetTeam(const ETeam& NewTeam)
{
	QSkillProgressBar->SetTeam(NewTeam);
	ESkillProgressBar->SetTeam(NewTeam);
	RMBSkillProgressBar->SetTeam(NewTeam);
}
