// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Occupation/Team.h"
#include "OccupyExpressWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetBarImage(const ETeam& Team);
	
private:
	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Anti_Bar;

	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Center_Bar;

	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> Pro_Bar;
};