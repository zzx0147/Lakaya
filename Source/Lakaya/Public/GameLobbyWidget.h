// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
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
	//UFUNCTION(BlueprintCallable)
	void ChangeSelectedCharacterImage(FString ImagePath);

	UFUNCTION()
	void OnClickedCitizenButton();

	UFUNCTION()
	void OnClickedGovernmentManButton();

	UFUNCTION()
	void OnClickedGangsterButton();



protected:

	UPROPERTY(VisibleAnywhere)
	UImage* SelectedCharacterImage;

	UPROPERTY(VisibleAnywhere)
	UButton* CitizenButton;

	UPROPERTY(VisibleAnywhere)
	UButton* GovernmentManButton;

	UPROPERTY(VisibleAnywhere)
	UButton* GangsterButton;

};
