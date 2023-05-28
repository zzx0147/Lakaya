#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Occupation/PlayerTeam.h"
#include "GameResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

// private:
// 	void OnChangeWinner(const EPlayerTeam& NewWinner);

public:
	TWeakObjectPtr<UImage> VictoryImage;
	TWeakObjectPtr<UImage> DefeatImage;
	TWeakObjectPtr<UTextBlock> AntiScore;
	TWeakObjectPtr<UTextBlock> ProScore;
};