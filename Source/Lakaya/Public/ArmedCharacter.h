// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "ArmedCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public AMovableCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	UInputMappingContext* WeaponControlContext;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	int8 WeaponContextPriority;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* FireStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* FireStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* AbilityStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* AbilityStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* ReloadStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* ReloadStopAction;

public:
	AArmedCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

private:
	void FireStart(const FInputActionValue& Value);
	void FireStop(const FInputActionValue& Value);
	void AbilityStart(const FInputActionValue& Value);
	void AbilityStop(const FInputActionValue& Value);
	void ReloadStart(const FInputActionValue& Value);
	void ReloadStop(const FInputActionValue& Value);
};
