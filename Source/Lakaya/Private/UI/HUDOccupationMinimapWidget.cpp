// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDOccupationMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"

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

	// 게임 중에는 미니맵을 매 프레임마다 업데이트를 해줘야 합니다.
	if (UpdateMinimap)
		UpdatePlayerPosition(CurrentTeam);
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

FVector2d UHUDOccupationMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
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

void UHUDOccupationMinimapWidget::UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const
{
	const auto PlayerCharacter = NewPlayerState.GetPlayerController()->GetCharacter();
	const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
	const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();
	
	ParentPanel->SetRenderTranslation(-NewPosition);
	RetainerBox->SetRenderTransformAngle(-(PlayerRotation.Yaw + 90.0f));
}