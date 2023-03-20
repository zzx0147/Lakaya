// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GamePlayHealthWidget.h"
#include "GamePlayBulletWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGamePlayBulletWidget : public UGamePlayHealthWidget
{
	GENERATED_BODY()
	
public:
	UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnChangeRemainBullets(int16 NewRemainBullets);
	void OnChangeMagazineCapacity(int16 NewMagazineCapacity);

public:

protected:

	UTextBlock* RemainBulletsText;
	UTextBlock* MagazineCapacityText;

	int16 RemainBullets;
	int16 MagazineCapacity;
};
