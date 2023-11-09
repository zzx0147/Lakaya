// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MovablePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ETC/OptionSave.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMovablePlayerController::AMovablePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Input/IC_CharacterControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MoveFinder(
		TEXT("InputAction'/Game/Input/IA_Move'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> LookFinder(
		TEXT("InputAction'/Game/Input/IA_Look'"));

	if (ContextFinder.Succeeded()) MovementContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;

	MouseSensitivity = 1.0f;
}

void AMovablePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FAsyncLoadGameFromSlotDelegate Delegate;
	
	Delegate.BindWeakLambda(this,
		[&](const FString& SlotName, const int32 UserIndex, USaveGame* SaveFile)
		{
			const auto OptionSaveFile = Cast<UOptionSave>(SaveFile);
			if (OptionSaveFile == nullptr) return;
			MouseSensitivity = OptionSaveFile->MouseSensitivity;
		}); // 람다식으로 바인딩
	
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("TestSaveSlot"), 0, Delegate);

}

void AMovablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Look);
}

void AMovablePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(MovementContext, MovementContextPriority);
}

void AMovablePlayerController::Move(const FInputActionValue& Value)
{
	if (const auto LocalCharacter = GetCharacter())
	{
		const auto Vector = Value.Get<FVector2D>();
		const FRotationMatrix Matrix((FRotator(0, GetControlRotation().Yaw, 0)));

		LocalCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::X), Vector.Y);
		LocalCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), Vector.X);
	}
}

void AMovablePlayerController::Look(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	AddYawInput(Vector.X * MouseSensitivity);
	AddPitchInput(Vector.Y * MouseSensitivity);
}
