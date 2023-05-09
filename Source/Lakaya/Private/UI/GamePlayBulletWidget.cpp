#define DO_CHECK 1

#include "UI/GamePlayBulletWidget.h"

#include "Components/TextBlock.h"

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

void UGamePlayBulletWidget::SetRemainBullet(const uint16& RemainBullet)
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBulletsText->SetText(FText::AsNumber(RemainBullet));
}

void UGamePlayBulletWidget::SetMaxBullet(const uint16& MaxBullet)
{
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), MaxBullet)));
}
