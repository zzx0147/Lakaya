// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/MovementComponent.h"
#include "AiDroneCharacter.generated.h"

UCLASS()
class LAKAYA_API AAiDroneCharacter : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAiDroneCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMovementComponent* MovementComponent; // Movement 컴포넌트 선언

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	const class ULakayaAttributeSet* AttributeSet;

};
