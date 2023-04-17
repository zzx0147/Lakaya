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

private:
	UFUNCTION()
	void OnChangeJoinedPlayers(const uint8& PlayerCount) const;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingWidgetText;

	uint8 MaxPlayerCount;
};
