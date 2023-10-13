#include "UI/GamePlayBulletWidget.h"

#include "Character/Ability/Attribute/LakayaAttributeSet.h"
#include "Components/TextBlock.h"

UGamePlayBulletWidget::UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	MaxBullet(40), RemainBullet(40)
{
}

void UGamePlayBulletWidget::SetRemainBullet(const uint16& NewRemainBullet)
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBullet = NewRemainBullet;
	RemainBulletsText->SetText(FText::AsNumber(RemainBullet));
	UpdateBulletProgressBar((float)RemainBullet / MaxBullet);
}

void UGamePlayBulletWidget::SetMaxBullet(const uint16& NewMaxBullet)
{
	//TODO: 텍스트 포맷을 따로 저장해두면 퍼포먼스를 높힐 수 있습니다.
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MaxBullet = NewMaxBullet;
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMaxBullet)));
	UpdateBulletProgressBar((float)RemainBullet / MaxBullet);
}

void UGamePlayBulletWidget::OnChangeMaxBulletAttribute(const FOnAttributeChangeData& NewValue)
{
	SetMaxBullet(NewValue.NewValue);
}

void UGamePlayBulletWidget::OnChangeCurrentBulletAttribute(const FOnAttributeChangeData& NewValue)
{
	SetRemainBullet(NewValue.NewValue);
}
