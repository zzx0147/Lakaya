// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "UI/OccupationCharacterSelectWidget.h"
#include "Components/VerticalBox.h"
#include "UI/ImageTextWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "UI/PlayerInfoWidget.h"

UOccupationCharacterSelectWidget::UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UImageTextWidget> PlayerNameWidgetClassFinder(
		TEXT("/Game/Blueprints/UMG/WBP_PlayerNameWidget.WBP_PlayerNameWidget_C"));
	PlayerInfoWidgetClass = PlayerNameWidgetClassFinder.Class;
}

void UOccupationCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerInfoVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("PlayerInfoPanel")));
}

void UOccupationCharacterSelectWidget::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);
	if (PlayerState == GetOwningPlayer()->GetPlayerState<APlayerState>()) return; //로컬 플레이어가 아닐 때에만 따진다

	//위젯을 생성하고 등록시킨다.

	if (const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		const auto PlayerNameWidget = CreateWidget<UPlayerInfoWidget>(this, PlayerInfoWidgetClass);
		PlayerInfoVerticalBox->AddChildToVerticalBox(PlayerNameWidget);
		PlayerNameWidget->SetPlayerName(PlayerState->GetPlayerName());
		PlayerNameWidget->SetPadding(FMargin(0.0f,50.0f,0.0f,0.0f));
		
		BasePlayerState->OnPlayerNameChanged.AddLambda([PlayerNameWidget](const FString& NewName)
		{
			PlayerNameWidget->SetPlayerName(NewName);
		});

		BasePlayerState->OnCharacterNameChanged.AddLambda(
			[PlayerNameWidget](ALakayaBasePlayerState* ArgBasePlayerState, const FName& ArgCharacterName)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, *FString::Printf(TEXT("Change Character %s"), *ArgCharacterName.ToString()));
				// CharacterSelectedPlayerListBoxMap[ArgCharacterName]->AddChildToVerticalBox(PlayerNameWidget);
				// PlayerNameWidget->SetPlayerName(ArgBasePlayerState->GetPlayerName());
				PlayerNameWidget->SetCharacterName(ArgCharacterName);
			}
		);
	}
}
