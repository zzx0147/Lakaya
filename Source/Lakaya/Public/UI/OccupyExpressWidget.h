// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OccupyExpressElementWidget.h"
#include "RadialProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "OccupyExpressWidget.generated.h"


/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupyExpressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE TObjectPtr<UOccupyExpressElementWidget>& GetAntiAreaBar() { return AntiAreaBar; }
	FORCEINLINE TObjectPtr<UOccupyExpressElementWidget>& GetCenterAreaBar() { return CenterAreaBar; }
	FORCEINLINE TObjectPtr<UOccupyExpressElementWidget>& GetProAreaBar() { return ProAreaBar; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOccupyExpressElementWidget> AntiAreaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOccupyExpressElementWidget> CenterAreaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOccupyExpressElementWidget> ProAreaBar;
};