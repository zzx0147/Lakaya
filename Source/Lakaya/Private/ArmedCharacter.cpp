// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmedCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AArmedCharacter::AArmedCharacter()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Yongwoo/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Fire'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Ability'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Reload'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireFinder.Succeeded()) FireAction = FireFinder.Object;
	if (AbilityFinder.Succeeded()) AbilityAction = AbilityFinder.Object;
	if (ReloadFinder.Succeeded()) ReloadAction = ReloadFinder.Object;
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
		InputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AArmedCharacter::Fire);
		InputComponent->BindAction(AbilityAction, ETriggerEvent::Triggered, this, &AArmedCharacter::Ability);
		InputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AArmedCharacter::Reload);
	}
}

void AArmedCharacter::Fire(const FInputActionValue& Value)
{
	//TODO: 무기 격발
}

void AArmedCharacter::Ability(const FInputActionValue& Value)
{
	//TODO: 무기 보조 능력 시전
}

void AArmedCharacter::Reload(const FInputActionValue& Value)
{
	//TODO: 무기 능력 시전
}
