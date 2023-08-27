// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerNameDisplayerWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Interface/TeamObjectInterface.h"


void UPlayerNameDisplayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!PlayerNameText) return;
	if (!PlayerCameraManager.IsValid()) return;

	const FVector Start = PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PlayerCameraManager->GetCameraRotation().Vector() * 10000000.0f;

	FHitResult Result;
	GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility);
	if (const auto Pawn = Cast<APawn>(Result.GetActor()))
	{
		if (const auto PlayerState = Pawn->GetPlayerState())
		{
			PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));

			if (MyTeam == ETeam::Individual)
			{
				PlayerNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
				return;
			}

			if (const auto TeamObject = Cast<ITeamObjectInterface>(PlayerState))
			{
				PlayerNameText->SetColorAndOpacity(TeamObject->GetTeam() == MyTeam
					                                   ? FSlateColor(FLinearColor::Blue)
					                                   : FSlateColor(FLinearColor::Red));
			}
			return;
		}
	}
	PlayerNameText->SetText(FText::GetEmpty());
}

void UPlayerNameDisplayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MyTeam = ETeam::Individual;

	PlayerCameraManager = GetOwningPlayerCameraManager();

	if (const auto PlayerPawn = GetOwningPlayerPawn())
		CollisionQueryParams.AddIgnoredActor(PlayerPawn);

	if (const auto PlayerState = Cast<ITeamObjectInterface>(GetOwningPlayerState()))
	{
		MyTeam = PlayerState->GetTeam();
	}
}
