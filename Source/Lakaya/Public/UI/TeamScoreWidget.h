// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Occupation/PlayerTeamState.h"
#include "TeamScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UTeamScoreWidget : public UMatchStateWidget
{
	GENERATED_BODY()

public:
	virtual bool OnMatchStart() override;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnChangeATeamScore(const float& NewScore) const;

	UFUNCTION()
	void OnChangeBTeamScore(const float& NewScore) const;

	void OnTeamScoreChanged(const EPlayerTeamState& Team, const float& Score) const;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ATeamScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BTeamScoreText;

	float MaxScore;
};
