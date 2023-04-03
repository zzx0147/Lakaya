// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MenuCallingPlayerController.h"

#include "Character/CollectorPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/IndividualGameState.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/LoadingWidget.h"
#include "Blueprint/UserWidget.h"

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

			#pragma region Update UI
			if (GetWorld()->GetGameState() == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("GetGameState is null."));
				return;
			}

			// 로딩 부분
			CreateLoadingWidget();
			CreateGameTimeWidget();
			CreateScoreBoardWidget();
			CreateGamePlayCrosshairWidget();
			CreateGamePlayHealthWidget();

			#pragma endregion 
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

void AMenuCallingPlayerController::CreateLoadingWidget()
{
	if (IsLocalPlayerController())
	{
		// 로딩 위젯
		UClass* LoadingWidgetClass = LoadClass<ULoadingWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_LoadingWidget.WBP_LoadingWidget_C"));
		if (LoadingWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadingWidgetClass is null."));
			return;
		}
			
		LoadingWidget = CreateWidget<ULoadingWidget>(this, LoadingWidgetClass);
		if (LoadingWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadingWidget is null."));
			return;
		}
	
		LoadingWidget->AddToViewport();
	}
}

void AMenuCallingPlayerController::CreateGameTimeWidget()
{
	if (IsLocalPlayerController())
	{
		// 로딩 위젯
		UClass* GameTimeWidgetClass = LoadClass<UGameTimeWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GameTimeWidget.WBP_GameTimeWidget_C"));
		if (GameTimeWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameTimeWidgetClass is null."));
			return;
		}
			
		GameTimeWidget = CreateWidget<UGameTimeWidget>(this, GameTimeWidgetClass);
		if (GameTimeWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameTimeWidget is null."));
			return;
		}
	
		GameTimeWidget->AddToViewport();
	}
}

void AMenuCallingPlayerController::CreateScoreBoardWidget()
{
	if (IsLocalController())
	{
		// 스코어보드 위젯
		UClass* ScoreBoardWidgetClass  = LoadClass<UGameScoreBoardWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GameScoreBoardWidget.WBP_GameScoreBoardWidget_C"));
		if (ScoreBoardWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ScoreBoardWidgetClass is null."));
			return;
		}
			
		GameScoreBoardWidget = CreateWidget<UGameScoreBoardWidget>(this, ScoreBoardWidgetClass );
		if (GameScoreBoardWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ScoreBoardWidget is null."));
			return;
		}
		GameScoreBoardWidget->AddToViewport();
	}
}

void AMenuCallingPlayerController::CreateGamePlayCrosshairWidget()
{
	if (IsLocalController())
	{
		// 스코어보드 위젯
		UClass* GamePlayCrosshairWidgetClass  = LoadClass<UGamePlayCrosshairWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GamePlayCrosshairWidget.WBP_GamePlayCrosshairWidget_C"));
		if (GamePlayCrosshairWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayCrosshairWidgetClass is null."));
			return;
		}
			
		GamePlayCrosshairWidget = CreateWidget<UGamePlayCrosshairWidget>(this, GamePlayCrosshairWidgetClass );
		if (GamePlayCrosshairWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayCrosshairWidget is null."));
			return;
		}
		GamePlayCrosshairWidget->AddToViewport();
	}
}

void AMenuCallingPlayerController::CreateGamePlayHealthWidget()
{
	if (IsLocalController())
	{
		// 스코어보드 위젯
		UClass* GamePlayHealthWidgetClass  = LoadClass<UGamePlayHealthWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget.WBP_GamePlayHealthWidget_C"));
		if (GamePlayHealthWidgetClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayHealthWidgetClass is null."));
			return;
		}
			
		GamePlayHealthWidget = CreateWidget<UGamePlayHealthWidget>(this, GamePlayHealthWidgetClass );
		if (GamePlayHealthWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayHealthWidget is null."));
			return;
		}
		
		GamePlayHealthWidget->AddToViewport();
	}
}