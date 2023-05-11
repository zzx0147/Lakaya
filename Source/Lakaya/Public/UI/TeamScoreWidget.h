#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "GameMode/OccupationGameState.h"
#include "Occupation/PlayerTeam.h"
#include "TeamScoreWidget.generated.h"

UCLASS()
class LAKAYA_API UTeamScoreWidget : public UMatchStateWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnChangeATeamScore(const float& NewScore) const;

	UFUNCTION()
	void OnChangeBTeamScore(const float& NewScore) const;

	UFUNCTION()
	void OnTeamScoreChanged(const EPlayerTeam& Team, const float& Score) const;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ATeamScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BTeamScoreText;

	float MaxScore;
};