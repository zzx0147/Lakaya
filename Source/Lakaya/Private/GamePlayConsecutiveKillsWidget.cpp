#define DO_CHECK 1

#include "GamePlayConsecutiveKillsWidget.h"
#include "Components/ProgressBar.h"

UGamePlayConsecutiveKillsWidget::UGamePlayConsecutiveKillsWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConsecutiveKills = 0;
	MaximumConsecutiveKills = 5;
}

void UGamePlayConsecutiveKillsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//�ʱ�ȭ �� ��üũ
#pragma region InitAndNullCheck

	ConsecutiveKillsProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ConsecutiveKills_Prb")));
	check(ConsecutiveKillsProgressBar != nullptr);

#pragma endregion
}

void UGamePlayConsecutiveKillsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills(int NewConsecutiveKills)
{
	//���ο� ����óġ ���� ������ ���α׷����� ������Ʈ
	ConsecutiveKills = NewConsecutiveKills;
	ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
}