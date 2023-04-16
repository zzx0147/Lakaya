#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/HUD.h"
#include "GameMode/IndividualGameMode.h"
#include "GameMode/IndividualGameState.h"
#include "GameMode/OccupationGameState.h"
#include "LoadingWidget.generated.h"

UCLASS()
class LAKAYA_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingWidgetText;

public:
	UFUNCTION()
	void ReMoveLoadingWidget(EOccupationGameState ChangeGamState);
	
public:
	UFUNCTION()
	void OnChangeJoinedPlayers(uint8 Number);

	AOccupationGameState* OccupationGameState;
};