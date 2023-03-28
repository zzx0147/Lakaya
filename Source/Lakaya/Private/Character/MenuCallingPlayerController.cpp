// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MenuCallingPlayerController.h"

#include "Character/CollectorPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerState.h"
#include "UI/GameScoreBoardWidget.h"

void AMenuCallingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const auto Component = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Component->BindAction(MenuAction, ETriggerEvent::Triggered, this, &AMenuCallingPlayerController::MenuHandler);
		Component->BindAction(LoadoutAction, ETriggerEvent::Triggered, this,
		                      &AMenuCallingPlayerController::LoadoutHandler);
		Component->BindAction(ScoreAction, ETriggerEvent::Triggered, this,
		                      &AMenuCallingPlayerController::ScoreHandler);
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
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Loadout'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ScoreFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ScoreBoard'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) LoadoutAction = WeaponFinder.Object;
	if (ScoreFinder.Succeeded()) ScoreAction = ScoreFinder.Object;
}

void AMenuCallingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (auto LocalPlayer = GetLocalPlayer())
	{
		if (const auto Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
			
			UClass* ScoreBoardWidget  = LoadClass<UGameScoreBoardWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GameScoreBoardWidget.WBP_GameScoreBoardWidget_C"));
			UGameScoreBoardWidget* newWidget = CreateWidget<UGameScoreBoardWidget>(this, ScoreBoardWidget );
			if (newWidget)
			{
				newWidget->AddToViewport();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ScoreBoardWidget is null."));
			}
		}
	}
}

void AMenuCallingPlayerController::MenuHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Menu"));
}

void AMenuCallingPlayerController::LoadoutHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("WeaponLoadout"));
}

void AMenuCallingPlayerController::ScoreHandler(const FInputActionValue& Value)
{
}