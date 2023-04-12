#define DO_CHECK 1

#include "UI/GamePlayConsecutiveKillsWidget.h"

#include "Components/ProgressBar.h"
#include "Weapon/WeaponComponent.h"


void UGamePlayConsecutiveKillsWidget::BindWeapon(UWeaponComponent* const& WeaponComponent)
{
	if (WeaponComponent)
	{
		WeaponComponent->OnUpgradeLevelChanged.AddUObject(
			this, &UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills);

		MaximumConsecutiveKills = WeaponComponent->GetMaximumUpgradeLevel();
		ConsecutiveKills = WeaponComponent->GetUpgradeLevel();
		ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
	}
}

void UGamePlayConsecutiveKillsWidget::UnBindWeapon(UWeaponComponent* const& WeaponComponent)
{
	if (WeaponComponent)
	{
		WeaponComponent->OnUpgradeLevelChanged.RemoveAll(this);

		ConsecutiveKills = MaximumConsecutiveKills = 0;
		ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
	}
}

void UGamePlayConsecutiveKillsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//초기화 후 널체크
#pragma region InitAndNullCheck

	ConsecutiveKillsProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ConsecutiveKills_Prb")));
	check(ConsecutiveKillsProgressBar != nullptr);

#pragma endregion
}

void UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills(const uint8& NewConsecutiveKills)
{
	//새로운 연속처치 값을 저장후 프로그래스바 업데이트
	ConsecutiveKills = NewConsecutiveKills;
	ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
}
