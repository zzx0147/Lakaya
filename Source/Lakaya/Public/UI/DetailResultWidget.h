#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "DetailResultWidget.generated.h"

UCLASS()
class LAKAYA_API UDetailResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	explicit UDetailResultWidget(const FObjectInitializer& ObjectInitializer);
	
	void RegisterPlayer(APlayerState* PlayerState);

private:
	virtual void NativeConstruct() override;

public:
	// DetailWidget에 각각의 플레이어 정보를 표시하는 엘리먼트의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDetailResultElementWidget> ElementClass;

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

	TWeakObjectPtr<UVerticalBox> AntiVerticalBox;
	TWeakObjectPtr<UVerticalBox> ProVerticalBox;
};