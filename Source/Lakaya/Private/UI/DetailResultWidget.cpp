#include "UI/DetailResultWidget.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameMode/OccupationGameState.h"
#include "UI/DetailResultElementWidget.h"

void UDetailResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AntiTextBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("Anti_TextBox_Image")));
	ProTextBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("Pro_TextBox_Image")));

	VictoryImage = Cast<UImage>(GetWidgetFromName(TEXT("Victory_Image")));
	DefeatImage = Cast<UImage>(GetWidgetFromName(TEXT("Defeat_Image")));

	AntiUserBoxNameImage = Cast<UImage>(GetWidgetFromName(TEXT("UserBox_Anti_Name_Image")));
	ProUserBoxNameImage = Cast<UImage>(GetWidgetFromName(TEXT("UserBox_Pro_Name_Image")));

	AntiUserBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("UserBox_Anti_Image")));
	ProUserBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("UserBox_Pro_Image")));

	AntiRenaImage = Cast<UImage>(GetWidgetFromName(TEXT("Rank_Anti_Rena_Image")));
	ProRenaImage = Cast<UImage>(GetWidgetFromName(TEXT("Rank_Pro_Rena_Image")));

	AntiWaziImage = Cast<UImage>(GetWidgetFromName(TEXT("Rank_Anti_Wazi_Image")));
	ProWaziImage = Cast<UImage>(GetWidgetFromName(TEXT("Rank_Pro_Wazi_Image")));

	PortraitRenaImage = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_Rena_Image")));
	PortraitWaziImage = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_Wazi_Image")));

	AntiTotalBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("TotalBox_Anti_Image")));
	ProTotalBoxImage = Cast<UImage>(GetWidgetFromName(TEXT("TotalBox_Pro_Image")));
	
	UserBoxCharacterNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserBox_CharacterName_Text")));

	UserBoxNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Player_Name_Text")));

	InfoBoxScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBox_Score_Text")));
	InfoBoxOccupationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBox_Occupation_Text")));
	InfoBoxKillText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBox_Kill_Text")));
	InfoBoxDeathText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBox_Death_Text")));

#pragma region Null Check
	if (AntiTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiTextBoxImage is null"));
	if (ProTextBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProTextBoxImage is null"));
	
	if (VictoryImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("VictoryImage is null"));
	if (DefeatImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("DefeatImage is null"));
	
	if (AntiUserBoxNameImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiUserBoxNameImage is null"));
	if (ProUserBoxNameImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProUserBoxNameImage is null"));
	
	if (ProUserBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProUserBoxImage is null"));
	if (AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiRenaImage is null"));
	
	if (AntiRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiRenaImage is null"));
	if (ProRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProRenaImage is null"));
	
	if (AntiWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiWaziImage is null"));
	if (ProWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProWaziImage is null"));
	
	if (PortraitRenaImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("PortraitRenaImage is null"));
	if (PortraitWaziImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("PortraitWaziImage is null"));

	if (AntiTotalBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiTotalBoxImage is null."));
	if (ProTotalBoxImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProTotalBoxImage is null."));
	
	if (UserBoxCharacterNameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("UserBoxCharacterNameText is null"));
	
	if (UserBoxNameText == nullptr) UE_LOG(LogTemp, Warning, TEXT("UserBoxNameText is null"));
	
	if (InfoBoxScoreText == nullptr) UE_LOG(LogTemp, Warning, TEXT("InfoBoxScoreText is null"));
	if (InfoBoxOccupationText == nullptr) UE_LOG(LogTemp, Warning, TEXT("InfoBoxOccupationText is null"));
	if (InfoBoxKillText == nullptr) UE_LOG(LogTemp, Warning, TEXT("InfoBoxKillText is null"));
	if (InfoBoxDeathText == nullptr) UE_LOG(LogTemp, Warning, TEXT("InfoBoxDeathText is null"));
#pragma endregion
}