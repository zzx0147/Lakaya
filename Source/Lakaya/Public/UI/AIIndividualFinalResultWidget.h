// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"

#include "AIIndividualFinalResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAIIndividualFinalResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UAIIndividualFinalResultWidget(const FObjectInitializer& ObjectInitializer);
	void SetMatchResultData(float KillScore, TArray<TWeakObjectPtr<ALakayaBasePlayerState>> PlayersByTeamMap);
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnNotifyMatchResult(bool IsWin);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UAIIndividualFinalResultElement> AIIndividualFinalResultElement1;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAIIndividualFinalResultElement> AIIndividualFinalResultElement2;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAIIndividualFinalResultElement> AIIndividualFinalResultElement3;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> ExitButton;
};
