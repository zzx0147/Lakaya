// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntroWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIntroWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayersData(TArray<TObjectPtr<APlayerState>> Players);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UIntroElement> IntroElementProClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UIntroElement> IntroElementAntiClass;

	UPROPERTY()
	TArray<TObjectPtr<UIntroElement>> ProElements;
	
	UPROPERTY()
	TArray<TObjectPtr<UIntroElement>> AntiElements;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> ProVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> AntiVerticalBox;
};
