#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DetailResultWidget.generated.h"

UCLASS()
class LAKAYA_API UDetailResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<UImage> AntiTextBoxImage;
	TWeakObjectPtr<UImage> ProTextBoxImage;

	TWeakObjectPtr<UImage> VictoryImage;
	TWeakObjectPtr<UImage> DefeatImage;
	
	TWeakObjectPtr<UImage> AntiUserBoxNameImage;
	TWeakObjectPtr<UImage> ProUserBoxNameImage;

	TWeakObjectPtr<UImage> AntiUserBoxImage;
	TWeakObjectPtr<UImage> ProUserBoxImage;

	TWeakObjectPtr<UImage> AntiRenaImage;
	TWeakObjectPtr<UImage> ProRenaImage;

	TWeakObjectPtr<UImage> AntiWaziImage;
	TWeakObjectPtr<UImage> ProWaziImage;

	TWeakObjectPtr<UImage> PortraitRenaImage;
	TWeakObjectPtr<UImage> PortraitWaziImage;

	TWeakObjectPtr<UImage> AntiTotalBoxImage;
	TWeakObjectPtr<UImage> ProTotalBoxImage;
	
	TWeakObjectPtr<UTextBlock> UserBoxCharacterNameText;

	TWeakObjectPtr<UTextBlock> UserBoxNameText;

	TWeakObjectPtr<UTextBlock> InfoBoxScoreText;
	TWeakObjectPtr<UTextBlock> InfoBoxOccupationText;
	TWeakObjectPtr<UTextBlock> InfoBoxKillText;
	TWeakObjectPtr<UTextBlock> InfoBoxDeathText;
};