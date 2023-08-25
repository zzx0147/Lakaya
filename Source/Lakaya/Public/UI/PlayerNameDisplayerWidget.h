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

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY()
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;
	
	FCollisionQueryParams CollisionQueryParams;

	ETeam MyTeam;
};
