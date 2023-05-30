// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "OccupationCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationCharacterSelectWidget : public UGameLobbyCharacterSelectWidget
{
	GENERATED_BODY()
public:
	UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	virtual void RegisterPlayer(APlayerState* PlayerState) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayerInfoWidget> PlayerInfoWidgetClass;

	TObjectPtr<class UVerticalBox> PlayerInfoVerticalBox;
};
