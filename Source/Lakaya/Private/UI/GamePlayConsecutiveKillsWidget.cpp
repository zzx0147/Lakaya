#define DO_CHECK 1

#include "UI/GamePlayConsecutiveKillsWidget.h"

#include "Character/ArmedCharacter.h"
#include "Components/ProgressBar.h"
#include "Weapon/WeaponComponent.h"


void UGamePlayConsecutiveKillsWidget::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (const auto OldCharacter = Cast<AArmedCharacter>(OldPawn))
	{
		if (const auto WeaponComponent = OldCharacter->GetPrimaryWeapon())
		{
			WeaponComponent->OnUpgradeLevelChanged.RemoveAll(this);
			MaximumConsecutiveKills = ConsecutiveKills = 0;
		}
	}

	if (const auto NewCharacter = Cast<AArmedCharacter>(NewPawn))
	{
		if (const auto WeaponComponent = NewCharacter->GetPrimaryWeapon())
		{
			WeaponComponent->OnUpgradeLevelChanged.AddUObject(
				this, &UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills);

			MaximumConsecutiveKills = WeaponComponent->GetMaximumUpgradeLevel();
			ConsecutiveKills = WeaponComponent->GetUpgradeLevel();
		}
	}
	
	ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
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

void UGamePlayConsecutiveKillsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills(const uint8& NewConsecutiveKills)
{
	//새로운 연속처치 값을 저장후 프로그래스바 업데이트
	ConsecutiveKills = NewConsecutiveKills;
	ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
}
