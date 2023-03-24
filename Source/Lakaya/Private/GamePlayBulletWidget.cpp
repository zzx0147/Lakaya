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

	//�ʱ�ȭ �� ��üũ
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
	//������Ʈ�� �Ѿ� ������ �����ϰ� �ؽ�Ʈ�� ǥ��
	RemainBullets = NewRemainBullets;
	RemainBulletsText->SetText(FText::AsNumber(NewRemainBullets));
}

void UGamePlayBulletWidget::OnChangeMagazineCapacity(int16 NewMagazineCapacity)
{
	//������Ʈ�� �ִ� �Ѿ� ������ �����ϰ� �ؽ�Ʈ�� ǥ��(�տ� /�� �ٿ��� ǥ��)
	MagazineCapacity = NewMagazineCapacity;
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMagazineCapacity)));
}
