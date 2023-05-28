#include "UI/GradeResultWidget.h"

#include "UI/GradeResultElementWidget.h"

UGradeResultWidget::UGradeResultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UGradeResultElementWidget::StaticClass();
}

void UGradeResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AntiTextBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("GradeResult_Anti_TextBox_Image")));
	ProTextBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("GradeResult_Pro_TextBox_Image")));
	VictoryImage = Cast<UImage>(GetWidgetFromName(TEXT("GradeResult_Victory_Image")));
	DefeatImage = Cast<UImage>(GetWidgetFromName(TEXT("GradeResult_Defeat_Image")));

	if (AntiTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiTextBoxImage is null")); 
	if (ProTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProTextBox is null")); 
	if (VictoryImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("VictoryImage is null")); 
	if (DefeatImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("DefeatImage is null")); 
}