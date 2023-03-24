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

void UGamePlayConsecutiveKillsWidget::OnChangeConsecutiveKills(int NewConsecutiveKills)
{
	//새로운 연속처치 값을 저장후 프로그래스바 업데이트
	ConsecutiveKills = NewConsecutiveKills;
	ConsecutiveKillsProgressBar->SetPercent((float)ConsecutiveKills / MaximumConsecutiveKills);
}