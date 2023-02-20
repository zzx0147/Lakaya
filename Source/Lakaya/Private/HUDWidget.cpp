#include "HUDWidget.h"
#include "Components/TextBlock.h"
#include "IndividualGameMode.h"

void UHUDWidget::BindMinute(AIndividualGameMode* GameMinute)
{
	CurrentMinute = GameMinute;
}

void UHUDWidget::BindSecond(AIndividualGameMode* GameSecond)
{
	CurrentSecond = GameSecond;
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TextMinute = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtMinutes")));
	TextSecond = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtSeconds")));
}

void UHUDWidget::UpdateMinute()
{
	TextMinute->SetText(FText::FromString(FString::FromInt(CurrentMinute->GetMinute())));
}

void UHUDWidget::UpdateSecond()
{
	TextSecond->SetText(FText::FromString(FString::FromInt(CurrentSecond->GetSecond())));
}