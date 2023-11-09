// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OptionSave.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOptionSave : public USaveGame
{
	GENERATED_BODY()

	UOptionSave();


public:
	UPROPERTY(BlueprintReadWrite)
	float MasterVolume;

	UPROPERTY(BlueprintReadWrite)
	float BGMVolume;
	
	UPROPERTY(BlueprintReadWrite)
	float EffectVolume;

	UPROPERTY(BlueprintReadWrite)
	float MouseSensitivity;
};
