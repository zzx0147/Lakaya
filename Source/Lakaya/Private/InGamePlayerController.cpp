// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		Subsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
}

void AInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const auto Component = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Component->BindAction(MenuAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::MenuHandler);
		Component->BindAction(WeaponLoadoutAction, ETriggerEvent::Triggered, this,
		                      &AInGamePlayerController::WeaponHandler);
		Component->BindAction(ArmorLoadoutAction, ETriggerEvent::Triggered, this,
		                      &AInGamePlayerController::ArmorHandler);
	}
}

AInGamePlayerController::AInGamePlayerController()
{
	InterfaceContextPriority = -100;
	
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Yongwoo/Input/IC_InterfaceControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MenuFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Menu'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> WeaponFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_WeaponLoadout'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ArmorFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_ArmorLoadout'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) WeaponLoadoutAction = WeaponFinder.Object;
	if (ArmorFinder.Succeeded()) ArmorLoadoutAction = ArmorFinder.Object;
}

void AInGamePlayerController::MenuHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}

void AInGamePlayerController::WeaponHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}

void AInGamePlayerController::ArmorHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
}
