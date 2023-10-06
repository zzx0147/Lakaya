// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/Team.h"
#include "PlayerNameDisplayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UPlayerNameDisplayerWidget : public UUserWidget
{
	GENERATED_BODY()

	explicit UPlayerNameDisplayerWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

public:
	virtual void SetTeam(const ETeam& NewTeam) { MyTeam = NewTeam; }

private:

	UPROPERTY(EditDefaultsOnly)
	FLinearColor MyTeamColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor EnemyTeamColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor IndividualColor;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY()
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;

	ETeam MyTeam;

	TWeakObjectPtr<APawn> MyPawn;
};
