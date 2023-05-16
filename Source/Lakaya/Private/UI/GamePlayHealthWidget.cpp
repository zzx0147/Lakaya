#define DO_CHECK 1

#include "UI/GamePlayHealthWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGamePlayHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//초기화 후 널체크
#pragma region InitAndNullCheck

	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar_Prb")));
	HealthText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Health_Text")));
	MaximumHealthText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaximumHealth_Text")));
	
	check(HealthText != nullptr);
	check(MaximumHealthText != nullptr);
	check(HealthProgressBar != nullptr);

#pragma endregion
}

void UGamePlayHealthWidget::SetCurrentHealth(const float& NewHealth)
{
	//업데이트된 체력을 저장하고 소수점을 버린 뒤 텍스트로 표기
	Health = NewHealth;
	HealthText->SetText(FText::AsNumber(floor(Health)));
	
	//체력 바 업데이트
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::SetMaximumHealth(const float& NewMaximumHealth)
{
	//업데이트된 최대 체력을 저장하고 소수점을 버린뒤 텍스트로 표기(맨 앞에 /를 붙여서 표기)
	MaximumHealth = NewMaximumHealth;
	MaximumHealthText->SetText(FText::FromString(FString::Printf(TEXT("/%.0f"), MaximumHealth)));
	
	//체력바 업데이트
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::UpdateHealthProgressBar() const
{
	//체력 바 업데이트
	HealthProgressBar->SetPercent(Health / MaximumHealth);
}