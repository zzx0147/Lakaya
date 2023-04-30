#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Components/TextBlock.h"
#include "LoadingWidget.generated.h"

UCLASS()
class LAKAYA_API ULoadingWidget : public UMatchStateWidget
{
	GENERATED_BODY()

public:
	virtual void OnBeginPlay() override;
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual void SetPlayerNumber(const uint8& PlayerCount);
	virtual void SetMaximumPlayerNumber(const uint8& PlayerCount);

private:
	UE_DEPRECATED(5.1, "This function will be removed, event not used, instead use setter")
	UFUNCTION()
	void OnChangeJoinedPlayers(const uint8& PlayerCount) const;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingWidgetText;

	uint8 MaxPlayerCount;
};
