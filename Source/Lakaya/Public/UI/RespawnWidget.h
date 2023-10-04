// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API URespawnWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	explicit URespawnWidget(const FObjectInitializer& ObjectInitializer);
	
	void StartRespawnProgress(const float& ReservedRespawnTime, const float& CurrentTime);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URadialProgressBar> RadialProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RemainTimeText;
	
	float MaxRespawnTime;
	float RemainRespawnTime;
	uint8 bUseTick : 1;
};
