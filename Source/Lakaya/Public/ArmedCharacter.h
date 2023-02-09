// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerCharacter.h"
#include "ArmedCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public ABasePlayerCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Input)
	UInputMappingContext* WeaponControlContext;

	UPROPERTY(EditAnywhere, Category=Input)
	int8 WeaponContextPriority;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* AbilityAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ReloadAction;

public:
	AArmedCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

private:
	void Fire(const FInputActionValue& Value);
	void Ability(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
};
