#define DO_CHECK 1

#include "UI/GamePlayConsecutiveKillsWidget.h"

#include "Components/ProgressBar.h"


// void UGamePlayConsecutiveKillsWidget::BindCharacter(ACharacter* const& Character)
// {
// 	Super::BindCharacter(Character);
// 	//TODO: 캐릭터에 연속처치 시스템을 추가하고, 위젯을 바인딩합니다.
// }
//
// bool UGamePlayConsecutiveKillsWidget::UnbindCharacter(ACharacter* const& Character)
// {
// 	Super::UnbindCharacter(Character);
// 	return true;
// }

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
