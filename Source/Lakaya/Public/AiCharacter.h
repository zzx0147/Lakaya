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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// AI 동작에 필요한 변수와 함수들을 선언합니다.
};

