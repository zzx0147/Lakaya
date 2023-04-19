#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode//OccupationGameState.h"
#include "InGameScoreBoardWidget.generated.h"

UCLASS()
class LAKAYA_API UInGameScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
};