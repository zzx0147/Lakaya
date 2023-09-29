// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePlayerController.h"
#include "Character/ArmedCharacter.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class LAKAYA_API ABattlePlayerController : public AMovablePlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController();

protected:
	virtual void SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent) override;
	virtual void SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem) override;
	virtual void OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn) override;

private:
	void StartAbility(EAbilityKind AbilityKind);
	void StopAbility(EAbilityKind AbilityKind);

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	UInputMappingContext* WeaponControlContext;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Context")
	int8 WeaponContextPriority;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* PrimaryStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* PrimaryStopAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* SecondStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* SecondStopAction;

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

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* DashStartAction;

	UPROPERTY(EditAnywhere, Category="Input|Weapon|Actions")
	UInputAction* DashStopAction;

	TWeakObjectPtr<AArmedCharacter> ArmedCharacter;
};
