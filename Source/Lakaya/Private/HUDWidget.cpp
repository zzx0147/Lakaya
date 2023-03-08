#include "HUDWidget.h"
#include "Components/TextBlock.h"
#include "IndividualGameMode.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MinuteText = Cast<UTextBlock>(GetWidgetFromName("txtMinutes"));
	SecondText = Cast<UTextBlock>(GetWidgetFromName("txtSeconds"));

	UpdateTimerText();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UHUDWidget::UpdateTimerText, 1.0f, true);

	AIndividualGameMode* GameMode = GetWorld()->GetAuthGameMode<AIndividualGameMode>();
	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode Null"));
	}

	// FText TimerMinutesText = FText::AsNumber(GameMode->GetMinutes(), &FNumberFormattingOptions().SetMinimumIntegralDigits(2).SetUseGrouping(false));
	// MinuteText->SetText(TimerMinutesText);
}

void UHUDWidget::UpdateTimerText()
{
	AIndividualGameMode* GameMode = GetWorld()->GetAuthGameMode<AIndividualGameMode>();
	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode Null"));
	}
	
	// if (GameMode->GetSeconds() > 0)
	// {
	// 	GameMode->MinusSeconds(1);
	// 	FText TimerValueText = FText::AsNumber(GameMode->GetSeconds(), &FNumberFormattingOptions().SetMinimumIntegralDigits(2).SetUseGrouping(false));
	// 	SecondText->SetText(TimerValueText);
	// }
	// else if (GameMode->GetMinutes() > 0)
	// {
	// 	GameMode->MinusMinutes(1);
	// 	GameMode->SetSeconds(59);
	// 	FText TimerSecondsText = FText::AsNumber(GameMode->GetSeconds(), &FNumberFormattingOptions().SetMinimumIntegralDigits(2).SetUseGrouping(false));
	// 	SecondText->SetText(TimerSecondsText);
	// 	FText TimerMinutesText = FText::AsNumber(GameMode->GetMinutes(), &FNumberFormattingOptions().SetMinimumIntegralDigits(2).SetUseGrouping(false));
	// 	MinuteText->SetText(TimerMinutesText);
	// }
	// else
	// {
	// 	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	// }
}