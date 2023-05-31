#include "UI/GradeResultWidget.h"

#include "UI/GradeResultElementWidget.h"

// ATeamBox->AddChildToVerticalBox(Element);

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
	HoriBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("Horizontal Box")));
	
	if (AntiTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiTextBoxImage is null")); 
	if (ProTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProTextBox is null")); 
	if (VictoryImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("VictoryImage is null")); 
	if (DefeatImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("DefeatImage is null")); 
	if (HoriBox == nullptr) UE_LOG(LogTemp, Warning, TEXT("HoriBox is null."));
}

void UGradeResultWidget::RegisterPlayer(APlayerState* PlayerState)
{
	const auto Element = CreateWidget<UGradeResultElementWidget>(this, ElementClass);
	if (Element == nullptr) UE_LOG(LogTemp, Warning, TEXT("UGradeResultElement is null."));
	HoriBox->AddChildToHorizontalBox(Element);
}