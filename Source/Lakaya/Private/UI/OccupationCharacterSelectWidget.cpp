// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "UI/OccupationCharacterSelectWidget.h"
#include "Components/VerticalBox.h"
#include "UI/ImageTextWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Button.h"
#include "UI/PlayerInfoWidget.h"

UOccupationCharacterSelectWidget::UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
}

void UOccupationCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInfoVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("PlayerInfoPanel")));
	// CharacterSelectButton = Cast<UButton>(GetWidgetFromName(TEXT("CharSelect_Btn")));

	// CharacterSelectButton->OnClicked.AddUniqueDynamic(this, &UOccupationCharacterSelectWidget::OnClickedCharacterSelectButton);
}

// void UOccupationCharacterSelectWidget::OnClickedCharacterSelectButton()
// {
// 	// TODO : 캐릭터 선택 버튼 누를 시, 캐릭터 선택 위젯 창 제거.
// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("OnClickedCharacterSelectButton"));
// 	SetVisibility(ESlateVisibility::Hidden);
// }

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
