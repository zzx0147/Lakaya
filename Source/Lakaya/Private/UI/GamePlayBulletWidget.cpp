#define DO_CHECK 1

#include "GamePlayBulletWidget.h"
#include "Components/TextBlock.h"


UGamePlayBulletWidget::UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	MagazineCapacity = 30;
	RemainBullets = 30;
}

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

void UGamePlayBulletWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayBulletWidget::OnChangeRemainBullets(int16 NewRemainBullets)
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBullets = NewRemainBullets;
	RemainBulletsText->SetText(FText::AsNumber(NewRemainBullets));
}

void UGamePlayBulletWidget::OnChangeMagazineCapacity(int16 NewMagazineCapacity)
{
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MagazineCapacity = NewMagazineCapacity;
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMagazineCapacity)));
}
