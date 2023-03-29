#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/HUD.h"
#include "GameMode/IndividualGameMode.h"
#include "LoadingWidget.generated.h"

UCLASS()
class LAKAYA_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxPlayersText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* JoinedPlayersText;

public:
	UFUNCTION()
	void InitMaxPlayers(int32 Number);
	UFUNCTION()
	void OnChangeJoinedPlayers(int32 Number);

	FTimerHandle TimerHandle;
};