// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlaySkillWidget.generated.h"

/**
 *
 */

class UProgressBar;

UCLASS()
class LAKAYA_API UGamePlaySkillWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlaySkillWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<UProgressBar*> SkillProgressBarArray;
};
