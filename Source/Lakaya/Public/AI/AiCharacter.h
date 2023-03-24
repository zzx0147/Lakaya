// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AiCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AAiCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAiCharacter();
	virtual void BeginPlay() override;
	
};

