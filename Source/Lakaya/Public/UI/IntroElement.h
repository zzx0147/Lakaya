// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntroElement.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIntroElement : public UUserWidget
{
	GENERATED_BODY()
public:
	explicit UIntroElement(const FObjectInitializer& ObjectInitializer);
	
	void SetPlayerData(bool IsLocal ,const FName& CharacterName, const FString& PlayerName);


private:

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> CharacterTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> LocalBackgroundImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CharacterImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;
};
