// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmedCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "WeaponComponent.h"

AArmedCharacter::AArmedCharacter()
{
	if (IsRunningDedicatedServer()) return;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ModeSwitchFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_SwitchFireMode'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStop'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireStartFinder.Succeeded()) FireStartAction = FireStartFinder.Object;
	if (FireStopFinder.Succeeded()) FireStopAction = FireStopFinder.Object;
	if (ModeSwitchFinder.Succeeded()) FireModeSwitchAction = ModeSwitchFinder.Object;
	if (AbilityStartFinder.Succeeded()) AbilityStartAction = AbilityStartFinder.Object;
	if (AbilityStopFinder.Succeeded()) AbilityStopAction = AbilityStopFinder.Object;
	if (ReloadStartFinder.Succeeded()) ReloadStartAction = ReloadStartFinder.Object;
	if (ReloadStopFinder.Succeeded()) ReloadStopAction = ReloadStopFinder.Object;
}

void AArmedCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (InputSystem.IsValid())
	{
		InputSystem->AddMappingContext(WeaponControlContext, WeaponContextPriority);
		WeaponComponent = Cast<UWeaponComponent>(GetComponentByClass(UWeaponComponent::StaticClass()));
	}
}

void AArmedCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (InputSystem.IsValid()) InputSystem->RemoveMappingContext(WeaponControlContext);
}

void AArmedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto CastedComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		CastedComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStart);
		CastedComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStop);
		CastedComponent->BindAction(FireModeSwitchAction, ETriggerEvent::Triggered, this,
		                            &AArmedCharacter::SwitchFireMode);
		CastedComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStart);
		CastedComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStop);
		CastedComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStart);
		CastedComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStop);
	}
}

void AArmedCharacter::FireStart(const FInputActionValue& Value)
{
	if (WeaponComponent.IsValid()) WeaponComponent->Call_FireStart();
}

void AArmedCharacter::FireStop(const FInputActionValue& Value)
{
	if (WeaponComponent.IsValid()) WeaponComponent->Call_FireStop();
}

void AArmedCharacter::SwitchFireMode(const FInputActionValue& Value)
{
	if (WeaponComponent.IsValid()) WeaponComponent->Call_SwitchSelector();
}

void AArmedCharacter::AbilityStart(const FInputActionValue& Value)
{
}

void AArmedCharacter::AbilityStop(const FInputActionValue& Value)
{
}

void AArmedCharacter::ReloadStart(const FInputActionValue& Value)
{
}

void AArmedCharacter::ReloadStop(const FInputActionValue& Value)
{
}
