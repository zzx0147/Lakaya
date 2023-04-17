#include "PlayerController/MenuCallingPlayerController.h"

#include "Character/CollectorPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerState.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/DirectionalDamageIndicator.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Character/ThirdPersonCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerStart.h"

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

	if (const auto Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		Subsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);

	if (APlayerState* CollPlayerState = GetPlayerState<APlayerState>())
	{
		if (ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(CollPlayerState))
		{
			InGameScoreBoardWidget->BindPlayerScore(CollectorPlayerState);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerState to ACollectorPlayerState"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get PlayerState from pawn or spectator"));
	}
}

void AMenuCallingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
#pragma region Update UI

		CreateLoadingWidget();
		CreateGameTimeWidget();
		// CreateScoreBoardWidget();
		CreateGamePlayCrosshairWidget();
		CreateTeamScoreWidget();
		CreateDirectionalDamageIndicator();
		CreateGameResultWidget();
		CreateInGameScoreBoardWidget();

#pragma endregion
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
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("ScoreBoard"));
	// DirectionalIndicatorImage->SetVisibility(ESlateVisibility::Visible);
	InGameScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMenuCallingPlayerController::CreateLoadingWidget()
{
	if (IsLocalController())
	{
		// 로딩 위젯
		LoadingWidget = CreateWidgetHelper<ULoadingWidget>(
			TEXT("/Game/Blueprints/UMG/WBP_LoadingWidget.WBP_LoadingWidget_C"));
		if (LoadingWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadingWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateGameTimeWidget()
{
	if (IsLocalController())
	{
		// 게임 시간 위젯
		GameTimeWidget = CreateWidgetHelper<UGameTimeWidget>(
			TEXT("/Game/Blueprints/UMG/WBP_GameTimeWidget.WBP_GameTimeWidget_C"));
		if (GameTimeWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameTimeWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateScoreBoardWidget()
{
	if (IsLocalController())
	{
		// 스코어보드 위젯
		GameScoreBoardWidget = CreateWidgetHelper<UGameScoreBoardWidget>(
			TEXT("/Game/Blueprints/UMG/WBP_GameScoreBoardWidget.WBP_GameScoreBoardWidget_C"));
		if (GameScoreBoardWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameScoreBoardWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateGamePlayCrosshairWidget()
{
	if (IsLocalController())
	{
		// 크로스헤어 위젯
		GamePlayCrosshairWidget = CreateWidgetHelper<UGamePlayCrosshairWidget>(
			TEXT("/Game/Blueprints/UMG/WBP_GamePlayCrosshairWidget.WBP_GamePlayCrosshairWidget_C"));
		if (GamePlayCrosshairWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GamePlayCrosshairWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateTeamScoreWidget()
{
	if (IsLocalController())
	{
		// 팀 스코어 위젯
		TeamScoreWidget = CreateWidgetHelper<UTeamScoreWidget>(
			TEXT("/Game/Blueprints/UMG/WBP_TeamScoreWidget.WBP_TeamScoreWidget_C"));
		if (TeamScoreWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("TeamScoreWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateGameResultWidget()
{
	if (IsLocalController())
	{
		// 게임결과창 위젯
		GameResultWidget = CreateWidgetHelper<UGameResultWidget>(TEXT("/Game/Blueprints/UMG/WBP_GameResultWidget.WBP_GameResultWidget_C"));
		if (GameResultWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameResultWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateInGameScoreBoardWidget()
{
	if (IsLocalController())
	{
		// 인게임스코어보드 위젯
		InGameScoreBoardWidget = CreateWidgetHelper<UInGameScoreBoardWidget>(TEXT("/Game/Blueprints/UMG/WBP_InGameScoreBoardWidget.WBP_InGameScoreBoardWidget_C"));
		if (InGameScoreBoardWidget == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("InGameScoreBoardWidget is null."));
			return;
		}
	}
}

void AMenuCallingPlayerController::CreateDirectionalDamageIndicator()
{
	if (IsLocalController())
	{
		// 팀 스코어 위젯
		DirectionalDamageIndicator = CreateWidgetHelper<UDirectionalDamageIndicator>(TEXT("/Game/Blueprints/UMG/WBP_DirectionalDamageIndicator.WBP_DirectionalDamageIndicator_C"));
		if (DirectionalDamageIndicator == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("DirectionalDamageIndicator is null."));
			return;
		}
	}
}



void AMenuCallingPlayerController::IndicateStart(FName CauserName, FVector DamageCursorPosition, float time)
{
	auto MyCharacter = GetCharacter();
	if (MyCharacter == nullptr) return;
	auto MyThirdPersonCharacter = Cast<AThirdPersonCharacter>(MyCharacter);
	if (MyThirdPersonCharacter == nullptr) return;

	DirectionalDamageIndicator->IndicateStart(CauserName, MyThirdPersonCharacter->GetCamera(), DamageCursorPosition, time);
}
