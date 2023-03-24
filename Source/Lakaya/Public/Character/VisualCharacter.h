// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "VisualCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AVisualCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	AVisualCharacter();

	// // 스켈레탈 메시 컴포넌트 추가
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	// USkeletalMeshComponent* SkeletalMeshComponent;
};
