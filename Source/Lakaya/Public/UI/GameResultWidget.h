#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Components/TextBlock.h"
#include "Occupation/PlayerTeam.h"
#include "GameResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void OnChangeWinner(const EPlayerTeam& NewWinner);

	UPROPERTY(meta =(BindWidget))
	UTextBlock* GameResultWidgetText;

	class AOccupationGameState* OccupationGameState;
};