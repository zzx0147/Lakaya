// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayKillLogWidget.generated.h"

/**
 *
 */

class UKillLogElement;

UCLASS()
class LAKAYA_API UGamePlayKillLogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<UKillLogElement*> KillLogElementArray;
};
