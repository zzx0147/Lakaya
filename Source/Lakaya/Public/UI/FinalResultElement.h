// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/Team.h"
#include "FinalResultElement.generated.h"


/**
 * 
 */
UCLASS()
class LAKAYA_API UFinalResultElement : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UFinalResultElement(const FObjectInitializer& ObjectInitializer);
	void SetGrade(const uint32& NewGrade);
	void SetTeam(const ETeam& NewTeam);
	void SetPlayer(TObjectPtr<class ALakayaBasePlayerState> NewPlayer);

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
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UserIdText;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, UTexture2D*> CursorImageMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, UTexture2D*> UserInfoImageMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, UTexture2D*> UserInfoOutlineImageMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, UMaterialInterface*> Character3DImageMaterialMap;
};
