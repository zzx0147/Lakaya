// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDOccupationMinimapWidget.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetSystemLibrary.h"

void UHUDOccupationMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RetainerCanvasPanel")));
	RetainerBox = Cast<URetainerBox>(GetWidgetFromName(TEXT("RetainerBox_74")));
	
	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);

	// TODO : 하드코딩이 아닌 GetDesiredSize() 함수를 이용해서 가져오도록 해야합니다.
	MinimapSize = FVector2D(250.0f, 381.0f);
}

void UHUDOccupationMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중에는 미니맵을 매 프레임마다 업데이트 해줍니다.
	if (UpdateMinimap)
	{
		// 자기 자신의 팀의 위치를 업데이트 해줍니다.
		UpdatePlayerPosition(CurrentTeam);
		
		// TODO : 와지 투시 스킬 사용중이거나, 시야에 적팀이 들어왔을 시에는, 상대방의 위치도 업데이트를 해줘야 합니다.
		// 적팀의 팀원 리스트를 가져옵니다.
		for (const auto& Enemy : PlayersByMinimap[CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti])
		{
			const auto& State = Enemy.Key;
		
			// TODO : 와지가 스킬 사용중일 때도 업데이트를 해줘야 합니다.
			if (IsInCameraView(State))
			{
				UE_LOG(LogTemp, Warning, TEXT("IsInCameraView"));
				UpdatePlayerPosition(CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti, State);
			}
			else
			{
				if (Enemy.Value->GetVisibility() != ESlateVisibility::Hidden)
					Enemy.Value->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

FVector2d UHUDOccupationMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

UImage* UHUDOccupationMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

	UCanvasPanelSlot* PanelSlot = ParentPanel->AddChildToCanvas(PlayerImage);
	if (PanelSlot == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PanelSlot is null."));
		return nullptr;
	}
	
	PanelSlot->SetAlignment(IconAlignment);
	PanelSlot->SetSize(IconSize);
	
	PlayerImage->SetVisibility(ESlateVisibility::Hidden);
	
	// 나 자신이라면 자신만의 아이콘으로 설정해줍니다.
	if (bMyPlayer)
	{
		PlayerImage->SetBrushFromTexture(OwnIcon);
		return PlayerImage;
	}

	// 나 자신이 아니라면 팀에 따라서 아이콘을 설정해줍니다.
	if (TeamIcons.Contains(Team))
	{
		PlayerImage->SetBrushFromTexture(TeamIcons[Team]);
		return PlayerImage;
	}

	UE_LOG(LogTemp, Warning, TEXT("HUDOccupationMinimapWidget_PlayerImage is null."));
	return nullptr;
}

void UHUDOccupationMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
	
	if (!MinimapImage) UE_LOG(LogTemp, Warning, TEXT("MinimapImage is null."));
	
	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);
		
		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);

		if (State == GetOwningPlayerState())
			UpdateMinimapImagePositionAndRotation(*State, NewPlayerPosition);
		
		// TODO : 맵과 이미지 사이즈가 확정이 되면 수정해야 합니다.
		// NewPlayerPosition + Widget Position = Player Position
		Image->SetRenderTranslation(NewPlayerPosition + FVector2D(125.f, 127.5f));
	}
}

void UHUDOccupationMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	if (!MinimapImage) UE_LOG(LogTemp, Warning, TEXT("MinimapImage is null."));

	if (const TWeakObjectPtr<ALakayaBasePlayerState> WeakNewPlayerState = NewPlayerState; !PlayersByMinimap[NewTeam].Contains(WeakNewPlayerState))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewPlayerState is not in PlayersByMinimap."));
		return;
	}

	const auto& EnemyImage = PlayersByMinimap[NewTeam][NewPlayerState].Get();
	if (EnemyImage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyImage is null."));
		return;
	}
	
	const FVector2D PlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
	const FVector2D NewPlayerPosition = const_cast<UHUDOccupationMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	if (EnemyImage->GetVisibility() == ESlateVisibility::Hidden)
		EnemyImage->SetVisibility(ESlateVisibility::Visible);

	EnemyImage->SetRenderTranslation(NewPlayerPosition + FVector2D(125.f, 127.5f));
}

void UHUDOccupationMinimapWidget::UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState,
	const FVector2D NewPosition) const
{
	Super::UpdateMinimapImagePositionAndRotation(NewPlayerState, NewPosition);
}

bool UHUDOccupationMinimapWidget::IsInCameraView(const TWeakObjectPtr<ALakayaBasePlayerState> State) const
{
#pragma region NullCheck
	if (!State.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("State is not valid."));
		return false;
	}

	const APawn* EnemyPawn = State->GetPawn();
	if (!EnemyPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyPawn is null."));
		return false;
	}
	
	const APlayerController* PlayerController = GetOwningPlayer();

	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController or PlayerCameraManager is null."));
		return false;
	}
#pragma endregion
	
	// 시야에 적이 들어왔는지 확인합니다.
	{
		const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector EnemyLocation = EnemyPawn->GetActorLocation();
		const FVector DirectionToTarget = (EnemyLocation - CameraLocation).GetSafeNormal();
		const FVector LookDirection = PlayerController->GetControlRotation().Vector();
		const float AngleThreshold = FMath::DegreesToRadians(90.0f);
		const float AngleBetweenVectors = FMath::Acos(FVector::DotProduct(DirectionToTarget.GetSafeNormal(), LookDirection));
		
		bool bIsVisible = AngleBetweenVectors <= AngleThreshold;

		// 시야에 적이 들어왔는지 확인합니다.
		if (!bIsVisible) return false;
		
		FHitResult HitResult;

		if (bool bIsObstructed = UKismetSystemLibrary::LineTraceSingle(
			this,
			CameraLocation,
			EnemyLocation,
			TraceTypeQuery1,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			HitResult,
			true); bIsObstructed && HitResult.GetActor() != EnemyPawn)
		{
			bIsVisible = false;
		}
		
		return bIsVisible;
	}
}
