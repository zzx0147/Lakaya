// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "GameLobbyWidget.generated.h"
/**
 * 
 */
UCLASS()
class LAKAYA_API UGameLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameLobbyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:

protected:
	UFUNCTION(BlueprintCallable);
	void ChangeSelectedCharacterImage(FString ImagePath);

protected:

	UPROPERTY()
	UImage* SelectedCharacterImage;
};
