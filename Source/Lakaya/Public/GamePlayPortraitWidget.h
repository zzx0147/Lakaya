// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayPortraitWidget.generated.h"

/**
 *
 */

class UImage;
class UTexture2D;

UCLASS()
class LAKAYA_API UGamePlayPortraitWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetCharacterPortrait(int32 CharacterNum);

private:
	UImage* CharacterPortraitImage;
	TArray<UTexture2D*> CharacterPortraitTextureArray;
};
