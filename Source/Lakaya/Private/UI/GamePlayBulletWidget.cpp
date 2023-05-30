#include "UI/GamePlayBulletWidget.h"

#include "Components/TextBlock.h"

void UGamePlayBulletWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	RemainBulletsText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RemainBullets_Text")));
	MagazineCapacityText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MagazineCapacity_Text")));

	RenaWeaponImage = Cast<UImage>(GetWidgetFromName("Rena_Weapon_Image"));
	WaziWeaponImage = Cast<UImage>(GetWidgetFromName("Wazi_Weapon_Image"));
	
	check(RemainBulletsText != nullptr);
	check(MagazineCapacityText != nullptr);
	check(RenaWeaponImage != nullptr);
	check(WaziWeaponImage != nullptr);
#pragma endregion
}

void UGamePlayBulletWidget::SetRemainBullet(const uint16& RemainBullet)
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBulletsText->SetText(FText::AsNumber(RemainBullet));
}

void UGamePlayBulletWidget::SetMaxBullet(const uint16& MaxBullet)
{
	//TODO: 텍스트 포맷을 따로 저장해두면 퍼포먼스를 높힐 수 있습니다.
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), MaxBullet)));
}
