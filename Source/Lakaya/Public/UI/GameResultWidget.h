#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowResult(const bool& IsWin, const float& AntiScore, const float& ProScore);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> VictoryPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> DefeatPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AntiScoreTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ProScoreTextBlock;
};