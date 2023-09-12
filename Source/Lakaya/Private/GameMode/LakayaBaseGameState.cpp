#include "GameMode/LakayaBaseGameState.h"

#include <filesystem>

#include "Character/LakayaBasePlayerState.h"
#include "EOS/EOSGameInstance.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BattlePlayerController.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
// #include "UI/GamePlayCrossHairWidget.h"
#include "NavigationSystemTypes.h"
#include "UI/DynamicCrossHairWidget.h"
#include "UI/GamePlayKillLogWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/PlayerNameDisplayerWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
	MatchDuration = 180.f;
	MatchWaitDuration = 30.0f;
	bWantsSendRecordResult = false;
}

void ALakayaBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBaseGameState, MatchEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, MatchWaitEndingTime);
}

void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();

	if (const auto LocalController = GetWorld()->GetFirstPlayerController();
		LocalController && LocalController->IsLocalController())
	{
		if (LoadingWidgetClass.Get() != nullptr)
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(LocalController, LoadingWidgetClass);
			if (LoadingWidget != nullptr)
			{
				LoadingWidget->SetMaximumPlayerNumber(MaximumPlayers);
				LoadingWidget->SetPlayerNumber(PlayerArray.Num());
				LoadingWidget->AddToViewport(10);
				LoadingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}

		if (ScoreBoardClass)
		{
			ScoreBoard = CreateWidget<UGameScoreBoardWidget>(LocalController, ScoreBoardClass);
			if (ScoreBoard.IsValid())
			{
				ScoreBoard->AddToViewport();
				ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
				for (auto& Player : PlayerArray) ScoreBoard->RegisterPlayer(Player);
			}
		}

		if (InGameTimerWidgetClass)
		{
			InGameTimeWidget = CreateWidget<UGameTimeWidget>(LocalController, InGameTimerWidgetClass);
			if (InGameTimeWidget.IsValid())
			{
				InGameTimeWidget->AddToViewport(10);
				InGameTimeWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		// if (CrosshairWidgetClass)
		// {
		// 	CrosshairWidget = CreateWidget<UGamePlayCrosshairWidget>(LocalController, CrosshairWidgetClass);
		// 	if (CrosshairWidget != nullptr)
		// 	{
		// 		CrosshairWidget->AddToViewport();
		// 		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		// 	}
		// }

		if (DynamicCrossHairWidgetClass)
		{
			DynamicCrossHairWidget = CreateWidget<UDynamicCrossHairWidget>(LocalController, DynamicCrossHairWidgetClass);
			if (DynamicCrossHairWidget != nullptr)
			{
				DynamicCrossHairWidget->AddToViewport();
				DynamicCrossHairWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
			
		if (KillLogWidgetClass)
		{
			KillLogWidget = CreateWidget<UGamePlayKillLogWidget>(LocalController, KillLogWidgetClass);
			if (KillLogWidget.IsValid())
			{
				KillLogWidget->AddToViewport();
				KillLogWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
		
		if (PlayerNameDisplayerWidgetClass)
		{
			PlayerNameDisplayerWidget = CreateWidget<UPlayerNameDisplayerWidget>(LocalController, PlayerNameDisplayerWidgetClass);
			if (PlayerNameDisplayerWidget.IsValid())
			{
				PlayerNameDisplayerWidget->AddToViewport();
				PlayerNameDisplayerWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}

#pragma region MiniMap
		// if (MiniMapWidgetClass)
		// {
		// 	MiniMapWidget = CreateWidget<UMiniMapWidget>(LocalController, MiniMapWidgetClass);
		// 	if (MiniMapWidget.IsValid())
		// 	{
		// 		MiniMapWidget->AddToViewport();
		// 		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
		//         
		// 		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
		// 		if (RenderTarget)
		// 		{
		// 			RenderTarget->InitAutoFormat(1024, 1024);
		// 			RenderTarget->UpdateResource();
		//
		// 			UE_LOG(LogTemp, Warning, TEXT("RenderTarget Initialized successfully."));
		//
		// 			USceneCaptureComponent2D* SceneCapture = NewObject<USceneCaptureComponent2D>(this);
		// 			if (SceneCapture)
		// 			{
		// 				SceneCapture->TextureTarget = RenderTarget;
		// 				SceneCapture->RegisterComponent();
		//
		// 				UE_LOG(LogTemp, Warning, TEXT("SceneCapture Initialized successfully."));
		//
		// 				UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(NULL, TEXT("'/Game/Characters/RenderTarget/MiniMap/M_Test.M_Test'"));
		// 				if (BaseMaterial)
		// 				{
		// 					UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		//
		// 					if (DynamicMaterialInstance)
		// 					{
		// 						DynamicMaterialInstance->SetTextureParameterValue(FName(TEXT("SceneTexture")), RenderTarget);
		//
		// 						UImage* MiniMapImage = MiniMapWidget->GetImageElement();
		// 						if (MiniMapImage)
		// 						{
		// 							FSlateBrush Brush;
		// 							Brush.SetResourceObject(DynamicMaterialInstance);
		// 							Brush.ImageSize.X = 1024.0f;
		// 							Brush.ImageSize.Y = 1024.0f;
		//
		// 							MiniMapImage->SetBrush(Brush);
		// 						}
		// 						else
		// 						{
		// 							UE_LOG(LogTemp, Warning, TEXT("MiniMapImage is null."));
		// 						}
		// 					}
		// 					else
		// 					{
		// 						UE_LOG(LogTemp, Warning, TEXT("DynamicMaterialInstance is null."));
		// 					}
		// 				}
		// 				else
		// 				{
		// 					UE_LOG(LogTemp, Warning, TEXT("BaseMaterial is null."));
		// 				}
		// 			}
		// 			else
		// 			{
		// 				UE_LOG(LogTemp, Warning, TEXT("SceneCapture is null."));
		// 			}
		// 		}
		// 		else
		// 		{
		// 			UE_LOG(LogTemp, Warning, TEXT("RenderTarget is null."));
		// 		}
		//
		// 	}
		// 	else
		// 	{
		// 		UE_LOG(LogTemp, Warning ,TEXT("Mini Map Widget is not valid."));
		// 	}    
		// }
		// else
		// {
		// 	UE_LOG(LogTemp ,Warning ,TEXT ("Minimap widget class is null"));
		// }

		// TODO : 아직 구현이 되지 않아 비활성화 합니다.
		// if (HelpWidgetClass)
		// {
		// 	HelpWidget = CreateWidget<UHelpWidget>(LocalController, HelpWidgetClass);
		// 	if (HelpWidget.IsValid())
		// 	{
		// 		HelpWidget->AddToViewport();
		// 		HelpWidget->SetVisibility(ESlateVisibility::Hidden);
		// 	}
		// }
#pragma endregion
	}
		
	SpawnOutlineManager();
}

void ALakayaBaseGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	if (ScoreBoard.IsValid()) ScoreBoard->RegisterPlayer(PlayerState);
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	StartTimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	StartTime = GetServerWorldTimeSeconds();

	if (GetCharacterSelectWidget())
	{
		CharacterSelectWidget->SetVisibility(ESlateVisibility::Visible);
	}

	if (LoadingWidget.IsValid())
		LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (InGameTimeWidget.IsValid())
	{
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	// if (CrosshairWidget != nullptr)
		// CrosshairWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (DynamicCrossHairWidget != nullptr)
		DynamicCrossHairWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	if (MiniMapWidget.IsValid())
	{
		MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget is null."));
	}
	
	// TODO : 아직 구현이 되지 않아 비활성화 합니다.
	// if (HelpWidget.IsValid())
	// 	HelpWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(MatchWaitToStartTimer, MatchWaitDuration, MatchWaitEndingTime, [this]
	{
		SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime, [this]
		{
			if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->EndMatch();
		}, InGameTimeWidget);
	}, InGameTimeWidget);
	
}

void ALakayaBaseGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(EndingTimer);
	if (ScoreBoard.IsValid()) ScoreBoard->RemoveFromParent();
	if (GetCharacterSelectWidget()) CharacterSelectWidget->RemoveFromParent();

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<AGameLobbyPlayerController>();
		LocalController && LocalController->IsLocalController())
		LocalController->SetEnableExitShortcut(true);

	const auto GameInstance = GetGameInstance();
	const auto EOSGameInstance = Cast<UEOSGameInstance>(GameInstance);
	if (EOSGameInstance == nullptr) return;
	if (HasAuthority())
	{
		EOSGameInstance->DestroySession();
	}
	else
	{
		EOSGameInstance->EndSession();
	}
}

void ALakayaBaseGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bWantsSendRecordResult)
	{
		bWantsSendRecordResult = !TrySendMatchResultData();
	}
}

void ALakayaBaseGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

#pragma region CharacterSelect
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// if (MatchState == MatchState::IsSelectCharacter)
	// {
		// HandleMatchIsCharacterSelect();
	// }
#pragma endregion CharacterSelect
}

void ALakayaBaseGameState::SetScoreBoardVisibility(const bool& Visible)
{
	if (MatchState == MatchState::InProgress) InternalSetScoreBoardVisibility(Visible);
}

UGameLobbyCharacterSelectWidget* ALakayaBaseGameState::GetCharacterSelectWidget()
{
	// 캐릭터 위젯이 존재하지 않는 경우 생성합니다.
	if (!CharacterSelectWidget.IsValid() && CharacterSelectWidgetClass)
	{
		CharacterSelectWidget = CreateWidget<UGameLobbyCharacterSelectWidget>(GetWorld(), CharacterSelectWidgetClass);
		if (CharacterSelectWidget != nullptr)
		{
			CharacterSelectWidget->AddToViewport();
			CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	return CharacterSelectWidget.Get();
}

void ALakayaBaseGameState::NotifyPlayerKilled_Implementation(APlayerState* VictimPlayer,
                                                             APlayerState* InstigatorPlayer, AActor* DamageCauser)
{
	OnPlayerKillNotified.Broadcast(VictimPlayer, InstigatorPlayer, DamageCauser);
	if (KillLogWidget.IsValid())
	{
		KillLogWidget->OnKillCharacterNotify(VictimPlayer, InstigatorPlayer, DamageCauser);
	}
}

void ALakayaBaseGameState::OnRep_MatchEndingTime()
{
	if (InGameTimeWidget.IsValid()) InGameTimeWidget->SetWidgetTimer(MatchEndingTime);
}

void ALakayaBaseGameState::OnRep_MatchWaitEndingTime()
{
	if (InGameTimeWidget.IsValid()) InGameTimeWidget->SetWidgetTimer(MatchWaitEndingTime);
}

bool ALakayaBaseGameState::SpawnOutlineManager()
{
	if (OutlineManager.IsValid())
		return true;

	if (OutlineManagerClass)
	{
		OutlineManager = GetWorld()->SpawnActor<AOutlineManager>(OutlineManagerClass);
		if (OutlineManager.IsValid())
			return true;
	}
	return false;
}

void ALakayaBaseGameState::ReserveSendRecord()
{
	bWantsSendRecordResult = true;
}

bool ALakayaBaseGameState::TrySendMatchResultData()
{
	return true;
}

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
                                            std::function<void(void)> Callback,
                                            TWeakObjectPtr<UGameTimeWidget> TimeWidget)
{
	GetWorldTimerManager().SetTimer(TimerHandle, Callback, Duration, false);
	if (HasAuthority())
	{
		EndingTime = GetServerWorldTimeSeconds() + Duration;
		if (TimeWidget.IsValid()) TimeWidget->SetWidgetTimer(EndingTime);
	}
}

void ALakayaBaseGameState::InternalSetScoreBoardVisibility(const bool& Visible)
{
	if (!ScoreBoard.IsValid()) return;
	ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

bool ALakayaBaseGameState::HasMatchStarted() const
{
	return Super::HasMatchStarted();
}