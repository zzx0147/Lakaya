// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogElement.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UKillLogElement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:
	void SetData();
};
