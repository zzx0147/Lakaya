// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCallingPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AMenuCallingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// In a dedicated server, the following logic is not necessary.
	if (IsRunningDedicatedServer()) return;

	if (const auto Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		Subsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
}

void AMenuCallingPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority()) return;
}

void AMenuCallingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const auto Component = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Component->BindAction(MenuAction, ETriggerEvent::Triggered, this, &AMenuCallingPlayerController::MenuHandler);
		Component->BindAction(WeaponLoadoutAction, ETriggerEvent::Triggered, this,
		                      &AMenuCallingPlayerController::WeaponHandler);
		Component->BindAction(ArmorLoadoutAction, ETriggerEvent::Triggered, this,
		                      &AMenuCallingPlayerController::ArmorHandler);
	}
}

AMenuCallingPlayerController::AMenuCallingPlayerController()
{
	if (IsRunningDedicatedServer()) return;

	InterfaceContextPriority = 100;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_InterfaceControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MenuFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Menu'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> WeaponFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_WeaponLoadout'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ArmorFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ArmorLoadout'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) WeaponLoadoutAction = WeaponFinder.Object;
	if (ArmorFinder.Succeeded()) ArmorLoadoutAction = ArmorFinder.Object;
}

void AMenuCallingPlayerController::MenuHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}

void AMenuCallingPlayerController::WeaponHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}

void AMenuCallingPlayerController::ArmorHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}
