// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmedCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AArmedCharacter::AArmedCharacter()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Yongwoo/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStartFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_FireStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStopFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_FireStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStartFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_AbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStopFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_AbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStartFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_ReloadStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStopFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_ReloadStop'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireStartFinder.Succeeded()) FireStartAction = FireStartFinder.Object;
	if (FireStopFinder.Succeeded()) FireStopAction = FireStopFinder.Object;
	if (AbilityStartFinder.Succeeded()) AbilityStartAction = AbilityStartFinder.Object;
	if (AbilityStopFinder.Succeeded()) AbilityStopAction = AbilityStopFinder.Object;
	if (ReloadStartFinder.Succeeded()) ReloadStartAction = ReloadStartFinder.Object;
	if (ReloadStopFinder.Succeeded()) ReloadStopAction = ReloadStopFinder.Object;
}

void AArmedCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (InputSystem) InputSystem->AddMappingContext(WeaponControlContext, WeaponContextPriority);
}

void AArmedCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (InputSystem) InputSystem->RemoveMappingContext(WeaponControlContext);
}

void AArmedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto InputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStart);
		InputComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStop);
		InputComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStart);
		InputComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStop);
		InputComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStart);
		InputComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStop);
	}
}

void AArmedCharacter::FireStart(const FInputActionValue& Value)
{
	//TODO: 무기 격발
}

void AArmedCharacter::FireStop(const FInputActionValue& Value)
{
}

void AArmedCharacter::AbilityStart(const FInputActionValue& Value)
{
	//TODO: 무기 보조 능력 시전
}

void AArmedCharacter::AbilityStop(const FInputActionValue& Value)
{
}

void AArmedCharacter::ReloadStart(const FInputActionValue& Value)
{
	//TODO: 무기 능력 시전
}

void AArmedCharacter::ReloadStop(const FInputActionValue& Value)
{
}
