#define DO_CHECK 1

#include "UI/GamePlayBulletWidget.h"

#include "Components/TextBlock.h"
// #include "Weapon/GunComponent.h"

void UGamePlayBulletWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	RemainBulletsText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RemainBullets_Text")));
	MagazineCapacityText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MagazineCapacity_Text")));

	check(RemainBulletsText != nullptr);
	check(MagazineCapacityText != nullptr);

#pragma endregion
}

void UGamePlayBulletWidget::BindWeapon(UGunComponent* const& GunComponent)
{
	// if (GunComponent)
	// {
	// 	GunComponent->OnCurrentBulletChanged.AddUObject(this, &UGamePlayBulletWidget::OnChangeRemainBullets);
	// 	GunComponent->OnMaximumBulletChanged.AddUObject(this, &UGamePlayBulletWidget::OnChangeMagazineCapacity);
	//
	// 	OnChangeRemainBullets(GunComponent->GetRemainBullets());
	// 	OnChangeMagazineCapacity(GunComponent->GetMagazineCapacity());
	// }
}

void UGamePlayBulletWidget::UnBindWeapon(UGunComponent* const& GunComponent)
{
	if (GunComponent)
	{
		// GunComponent->OnCurrentBulletChanged.RemoveAll(this);
		// GunComponent->OnMaximumBulletChanged.RemoveAll(this);

		OnChangeRemainBullets(0);
		OnChangeMagazineCapacity(0);
	}
}

void UGamePlayBulletWidget::OnChangeRemainBullets(const uint16& NewRemainBullets)
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBulletsText->SetText(FText::AsNumber(NewRemainBullets));
}

void UGamePlayBulletWidget::OnChangeMagazineCapacity(const uint16& NewMagazineCapacity)
{
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMagazineCapacity)));
}
