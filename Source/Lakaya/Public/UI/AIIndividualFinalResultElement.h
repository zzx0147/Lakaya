// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIIndividualFinalResultElement.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAIIndividualFinalResultElement : public UUserWidget
{
	GENERATED_BODY()
	
	public:
    	explicit UAIIndividualFinalResultElement(const FObjectInitializer& ObjectInitializer);
    	void SetGrade(const uint32& NewGrade);
    	void SetPlayer(TWeakObjectPtr<class ALakayaBasePlayerState> NewPlayer);
    
    protected:
    	virtual void NativeConstruct() override;
    
    private:
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<class UImage> Character3DImage;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<UImage> CursorImage;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<UImage> UserInfoBoxImage;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<UImage> UserInfoBoxOutlineImage;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<class UTextBlock> GradeText;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<UTextBlock> KillScoreText;
    
    	UPROPERTY(meta = (BindWidget))
    	TObjectPtr<UTextBlock> UserIdText;
    
    	UPROPERTY(EditDefaultsOnly)
    	TMap<FName, UTexture2D*> Character3DImageMaterialMap;
};
