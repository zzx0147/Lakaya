// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Occupation/Team.h"
#include "FinalResultWidget.generated.h"

UCLASS()
class LAKAYA_API UFinalResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UFinalResultWidget(const FObjectInitializer& ObjectInitializer);
	void SetTeam(const ETeam& NewTeam);
	void SetMatchResultData(ETeam WinTeam, float ProScore, float AntiScore, TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> PlayersByTeamMap);
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	virtual void NativeConstruct() override;

private:
	ETeam MyTeam;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> VictoryResultTexture;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> DefeatResultTexture;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, UTexture2D*> ButtonTextureMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, UTexture2D*> ButtonPressedTextureMap;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> FinalResultImage;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> ProScoreText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AntiScoreText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UFinalResultElement> FinalResultElement1;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFinalResultElement> FinalResultElement2;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFinalResultElement> FinalResultElement3;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> ExitButton;
	
};
