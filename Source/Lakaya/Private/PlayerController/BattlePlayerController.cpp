// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BattlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/ArmedCharacter.h"
#include "Character/Ability/CharacterAbility.h"
#include "UI/SkillProgressBar.h"
#include "UI/SkillWidget.h"


ABattlePlayerController::ABattlePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStartFinder(
		TEXT("InputAction'/Game/Input/IA_FireStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStopFinder(
		TEXT("InputAction'/Game/Input/IA_FireStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStartFinder(
		TEXT("InputAction'/Game/Input/IA_AbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStopFinder(
		TEXT("InputAction'/Game/Input/IA_AbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStartFinder(
		TEXT("InputAction'/Game/Input/IA_ReloadStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStopFinder(
		TEXT("InputAction'/Game/Input/IA_ReloadStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> PrimaryStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_PrimaryAbilityStart.IA_PrimaryAbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> PrimaryStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_PrimaryAbilityStop.IA_PrimaryAbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> SecondaryStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_SecondaryAbilityStart.IA_SecondaryAbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> SecondaryStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_SecondaryAbilityStop.IA_SecondaryAbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> DashStartFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_DashStart.IA_DashStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> DashStopFinder(TEXT(
		"/Script/EnhancedInput.InputAction'/Game/Input/IA_DashStop.IA_DashStop'"));

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
	                                   &ABattlePlayerController::StartAbility, Primary);
	EnhancedInputComponent->BindAction(PrimaryStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, Primary);
	EnhancedInputComponent->BindAction(SecondStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StartAbility, Secondary);
	EnhancedInputComponent->BindAction(SecondStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, Secondary);
	EnhancedInputComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StartAbility, WeaponFire);
	EnhancedInputComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, WeaponFire);
	EnhancedInputComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StartAbility, WeaponAbility);
	EnhancedInputComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, WeaponAbility);
	EnhancedInputComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StartAbility, WeaponReload);
	EnhancedInputComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, WeaponReload);
	EnhancedInputComponent->BindAction(DashStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StartAbility, Dash);
	EnhancedInputComponent->BindAction(DashStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::StopAbility, Dash);
}

void ABattlePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(WeaponControlContext, WeaponContextPriority);
}

void ABattlePlayerController::OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn)
{
	Super::OnPossessedPawnChangedCallback(ArgOldPawn, NewPawn);
	if (!IsLocalController()) return;
	ArmedCharacter = Cast<AArmedCharacter>(NewPawn);
}

void ABattlePlayerController::StartAbility(EAbilityKind AbilityKind)
{
	//TODO: 어빌리티 컨텍스트 개념 추가. 즉시 StartAbility를 호출하는 것이 아니도록 해야함
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(AbilityKind);
}

void ABattlePlayerController::StopAbility(EAbilityKind AbilityKind)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(AbilityKind);
}
