// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHealthWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UProgressBar;

UCLASS()
class LAKAYA_API UGamePlayHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayHealthWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnChangeHealth(AActor* Character, const float& NewHealth);
	void OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth);
	void UpdateHealthProgressBar();

private:
	UProgressBar* HealthProgressBar;
	UTextBlock* HealthText;
	UTextBlock* MaximumHealthText;

	float MaximumHealth;
	float Health;
};
