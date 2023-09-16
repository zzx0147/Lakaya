// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "UI/OccupationCharacterSelectWidget.h"
#include "Components/VerticalBox.h"
#include "UI/ImageTextWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/PlayerInfoWidget.h"

UOccupationCharacterSelectWidget::UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	TeamInfoTextTextureMap.Emplace(ETeam::Anti, nullptr);
	TeamInfoTextTextureMap.Emplace(ETeam::Pro, nullptr);

	CharacterSelectTextTextureMap.Emplace(ETeam::Anti, nullptr);
	CharacterSelectTextTextureMap.Emplace(ETeam::Pro, nullptr);

	GangrimButtonDisabledTextureMap.Emplace(ETeam::Anti, nullptr);
	GangrimButtonDisabledTextureMap.Emplace(ETeam::Pro, nullptr);

	RenaButtonDisabledTextureMap.Emplace(ETeam::Anti, nullptr);
	RenaButtonDisabledTextureMap.Emplace(ETeam::Pro, nullptr);

	WaziButtonDisabledTextureMap.Emplace(ETeam::Anti, nullptr);
	WaziButtonDisabledTextureMap.Emplace(ETeam::Pro, nullptr);

	AntiCharacterNameTextureMap.Emplace(CharacterNameArray[0],nullptr);
	AntiCharacterNameTextureMap.Emplace(CharacterNameArray[1],nullptr);
	AntiCharacterNameTextureMap.Emplace(CharacterNameArray[2],nullptr);

	ProCharacterNameTextureMap.Emplace(CharacterNameArray[0],nullptr);
	ProCharacterNameTextureMap.Emplace(CharacterNameArray[1],nullptr);
	ProCharacterNameTextureMap.Emplace(CharacterNameArray[2],nullptr);
}

void UOccupationCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInfoVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("PlayerInfoPanel")));

}

void UOccupationCharacterSelectWidget::SelectCharacter(const uint8& CharacterNum)
{
	Super::SelectCharacter(CharacterNum);

	if(MyTeam == ETeam::Anti)
	{
		CharacterNameImage->SetBrushFromTexture(AntiCharacterNameTextureMap[CharacterNameArray[CharacterNum]]);
	}
	else
	{
		CharacterNameImage->SetBrushFromTexture(ProCharacterNameTextureMap[CharacterNameArray[CharacterNum]]);
	}
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

void UOccupationCharacterSelectWidget::SetTeam(const ETeam& NewTeam)
{
	MyTeam = NewTeam;

	TeamInfoTextImage->SetBrushFromTexture(TeamInfoTextTextureMap[MyTeam]);
	CharacterSelectTextImage->SetBrushFromTexture(CharacterSelectTextTextureMap[MyTeam]);
	
	FButtonStyle ButtonStyle = CharacterButtonArray[0]->GetStyle();
	ButtonStyle.Disabled.SetResourceObject(RenaButtonDisabledTextureMap[MyTeam]);
	CharacterButtonArray[0]->SetStyle(ButtonStyle);

	ButtonStyle = CharacterButtonArray[1]->GetStyle();
	ButtonStyle.Disabled.SetResourceObject(WaziButtonDisabledTextureMap[MyTeam]);
	CharacterButtonArray[1]->SetStyle(ButtonStyle);

	ButtonStyle = CharacterButtonArray[2]->GetStyle();
	ButtonStyle.Disabled.SetResourceObject(GangrimButtonDisabledTextureMap[MyTeam]);
	CharacterButtonArray[2]->SetStyle(ButtonStyle);

	OnClickedCharacter1Button();
}
