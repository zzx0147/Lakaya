#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"
#include "Occupation/Team.h"
#include "TeamScoreWidget.generated.h"

UCLASS()
class LAKAYA_API UTeamScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UTeamScoreWidget(const FObjectInitializer& ObjectInitializer);

	void SetTeamScore(const ETeam& Team,const float& NewScore);
	void SetMaxScore(const float& ArgMaxScore);
	void SetMaxScoreVisibility(const bool& IsVisible);
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AntiTeamScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ProTeamScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxScoreText;
	
	float MaxScore;

	FText ScoreFormatText;
};