// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHealthWidget.generated.h"


class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class LAKAYA_API UGamePlayHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayHealthWidget(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
protected:


	UFUNCTION(BlueprintCallable)
	void OnChangeHealth(AActor* Character, const float& NewHealth);
	UFUNCTION(BlueprintCallable)
	void OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth);
	
	void UpdateHealthProgressBar();


private:

private:
	UProgressBar* HealthProgressBar;
	UTextBlock* HealthText;
	UTextBlock* MaximumHealthText;

private:
	float MaximumHealth;
	float Health;
};
