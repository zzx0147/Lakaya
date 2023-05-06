// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BattlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/ArmedCharacter.h"


ABattlePlayerController::ABattlePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> PrimaryStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_PrimaryAbilityStart.IA_PrimaryAbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> PrimaryStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_PrimaryAbilityStop.IA_PrimaryAbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> SecondaryStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_SecondaryAbilityStart.IA_SecondaryAbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> SecondaryStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_SecondaryAbilityStop.IA_SecondaryAbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> DashStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_DashStart.IA_DashStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> DashStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_DashStop.IA_DashStop'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireStartFinder.Succeeded()) FireStartAction = FireStartFinder.Object;
	if (FireStopFinder.Succeeded()) FireStopAction = FireStopFinder.Object;
	if (AbilityStartFinder.Succeeded()) AbilityStartAction = AbilityStartFinder.Object;
	if (AbilityStopFinder.Succeeded()) AbilityStopAction = AbilityStopFinder.Object;
	if (ReloadStartFinder.Succeeded()) ReloadStartAction = ReloadStartFinder.Object;
	if (ReloadStopFinder.Succeeded()) ReloadStopAction = ReloadStopFinder.Object;
	if (PrimaryStartFinder.Succeeded()) PrimaryStartAction = PrimaryStartFinder.Object;
	if (PrimaryStopFinder.Succeeded()) PrimaryStopAction = PrimaryStopFinder.Object;
	if (SecondaryStartFinder.Succeeded()) SecondStartAction = SecondaryStartFinder.Object;
	if (SecondaryStopFinder.Succeeded()) SecondStopAction = SecondaryStopFinder.Object;
	if (DashStartFinder.Succeeded()) DashStartAction = DashStartFinder.Object;
	if (DashStopFinder.Succeeded()) DashStopAction = DashStopFinder.Object;
}

void ABattlePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(PrimaryStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::PrimaryStart);
	EnhancedInputComponent->BindAction(PrimaryStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::PrimaryStop);
	EnhancedInputComponent->BindAction(SecondStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::SecondStart);
	EnhancedInputComponent->BindAction(SecondStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::SecondStop);
	EnhancedInputComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::FireStart);
	EnhancedInputComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::FireStop);
	EnhancedInputComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::AbilityStart);
	EnhancedInputComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::AbilityStop);
	EnhancedInputComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::ReloadStart);
	EnhancedInputComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::ReloadStop);
	EnhancedInputComponent->BindAction(DashStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::DashStart);
	EnhancedInputComponent->BindAction(DashStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::DashStop);
}

void ABattlePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(WeaponControlContext, WeaponContextPriority);
}

void ABattlePlayerController::OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn)
{
	Super::OnPossessedPawnChangedCallback(ArgOldPawn, NewPawn);
	if (IsLocalController()) ArmedCharacter = Cast<AArmedCharacter>(NewPawn);
}

void ABattlePlayerController::PrimaryStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(Primary);
}

void ABattlePlayerController::PrimaryStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(Primary);
}

void ABattlePlayerController::SecondStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(Secondary);
}

void ABattlePlayerController::SecondStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(Secondary);
}

void ABattlePlayerController::FireStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponFire);
}

void ABattlePlayerController::FireStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponFire);
}

void ABattlePlayerController::AbilityStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponAbility);
}

void ABattlePlayerController::AbilityStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponAbility);
}

void ABattlePlayerController::ReloadStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponReload);
}

void ABattlePlayerController::ReloadStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponReload);
}

void ABattlePlayerController::DashStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(Dash);
}

void ABattlePlayerController::DashStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(Dash);
}
