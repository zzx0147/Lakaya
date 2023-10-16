// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniMapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "GameMode/OccupationGameState.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateMinimap = false;
	
	IconAlignment = FVector2D(0.5f, 0.5f);
	IconSize = FVector2D(62.0f, 112.0f);
	// IconSize = FVector2D(31.0f, 56.0f);
	
	PlayersByMinimap.Emplace(ETeam::Anti);
	PlayersByMinimap.Emplace(ETeam::Pro);
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중이 아닐때에는 미니맵을 업데이트 해주지 않습니다.
	if (!UpdateMinimap) return;

	// 자신의 팀(자기 자신 포함)위치 를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);
	
	const ETeam EnemyTeam = CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti;

	if (const auto& GameState = Cast<AOccupationGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetbIsClairvoyanceActivated())
		{
			UpdatePlayerPosition(EnemyTeam);
			return;
		}
	}
	
	// TODO : 와지 투시 스킬을 사용했을 때도 추가해줘야 합니다.
	// TODO : 적을 발견했다면, 시야를 팀원들과 공유해야 합니다.
	// 적들의 위치정보를 가져와서, 내 시야에 들어와있다면, 미니맵에 표시해줍니다.
	for (const auto& Enemy : PlayersByMinimap[EnemyTeam])
	{
		const auto& EnemyState = Enemy.Key;
		const auto& EnemyImage = Enemy.Value;
		
		// 모든 아군을 순회해서 시야에 없다면, 적을 업데이트 하지 않습니다.
		for (const auto& Ally : PlayersByMinimap[CurrentTeam])
		{
			const auto& AllyState = Ally.Key;
			ALakayaBaseCharacter* AllyCharacter = Cast<ALakayaBaseCharacter>(AllyState->GetPawn());
			AllyCharacter->IsEnemyVisibleInCamera(EnemyTeam, EnemyState, EnemyImage);
		}
	}
}

FVector2D UMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
                                                           const FVector2D& MiniMapSize)
{
	// WorldMap Size.
	const FVector2D WorldMapSize(7560.0f, 10600.0f);
	
	const float XRatio = (PlayerLocation.X) / (WorldMapSize.X / MiniMapSize.X);
	const float YRatio = (PlayerLocation.Y) / (WorldMapSize.Y / MiniMapSize.Y);
	
	const FVector2D MiniMapCoordinates(XRatio, YRatio);
	
	return MiniMapCoordinates;
}

// TODO : 개인전에서는 나 자신만의 위치를 업데이트 해야 하며, 발견된 적의 위치만을 업데이트 해야 해서, 로직 변경이 필요합니다.
void UMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;
		
		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);
		
		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);

		Image->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

		// 검사한 상대가 나 자신이라면
		if (State == GetOwningPlayerState())
		{
			// 회전값을 업데이트 해줍니다.
			const auto PlayerCharacter = State->GetPlayerController()->GetCharacter();
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();
			Image->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		}
	}
}