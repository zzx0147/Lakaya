#include "UI/GameResultWidget.h"

#include "GameMode/OccupationGameState.h"

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VictoryImage = Cast<UImage>(GetWidgetFromName(TEXT("Result_Victory_Image")));
	DefeatImage = Cast<UImage>(GetWidgetFromName(TEXT("Result_Defeat_Image")));
	AntiScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Result_Anti_Score_Text")));
	ProScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Result_Pro_Score_Text")));

	if (VictoryImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("VictoryImage is null."));
	if (DefeatImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("DefeatImage is null."));
	if (AntiScore == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiScore is null."));
	if (ProScore == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProScore is null."));
}