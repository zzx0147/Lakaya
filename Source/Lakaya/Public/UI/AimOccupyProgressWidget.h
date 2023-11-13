// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "AimOccupyProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAimOccupyProgressWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetAimOccupyProgressBar(const ETeam& NewPlayerTeam, const float& NewProgress, const bool& bIsNewOccupy);
	void OccupyCrash() const;
	void InitAimOccupyWidget() const;
	void OccupySuccess();

	FORCEINLINE const TObjectPtr<UTexture2D>& GetOccupyChargeAntiTexture() const { return OccupyChargeAntiTexture; }
	FORCEINLINE const TObjectPtr<UTexture2D>& GetOccupyChargeProTexture() const { return OccupyChargeProTexture; }
	FORCEINLINE const ETeam& GetCurrentTeam() const { return CurrentTeam; }

	void SetCurrentTeam(const ETeam& NewTeam);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IngTextImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> FinishTextImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CrashTextImage;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> OccupyChargeAntiTexture;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> OccupyChargeProTexture;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* AimOccupyProgressbar;
	
	UPROPERTY()
	float Percent;

	ETeam CurrentTeam = ETeam::None;

	FTimerHandle TimerHandle_FinishText;
};