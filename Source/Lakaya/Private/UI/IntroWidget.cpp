// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IntroWidget.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/VerticalBox.h"
#include "UI/IntroElement.h"

void UIntroWidget::SetPlayersData(TArray<TObjectPtr<APlayerState>> Players)
{
	if(!IntroElementAntiClass || !IntroElementProClass) return;
	
	for(const auto PlayerState : Players)
	{
		const auto LakayaPlayerState = Cast<ALakayaBasePlayerState>(PlayerState);
		if(!LakayaPlayerState) return;

		if (const auto PlayerController = LakayaPlayerState->GetPlayerController(); PlayerController && PlayerController->IsLocalPlayerController())
		{
			if (LakayaPlayerState->GetTeam() == ETeam::Pro)
			{
				UIntroElement* IntroElement = NewObject<UIntroElement>(this, IntroElementProClass);
				ProElements.Emplace(IntroElement);
				ProVerticalBox->AddChildToVerticalBox(IntroElement);
				IntroElement->SetPlayerData(true, LakayaPlayerState->GetCharacterName(),LakayaPlayerState->GetPlayerName());
			}
			else
			{
				UIntroElement* IntroElement = NewObject<UIntroElement>(this, IntroElementAntiClass);
				AntiElements.Emplace(IntroElement);
				AntiVerticalBox->AddChildToVerticalBox(IntroElement);
				IntroElement->SetPlayerData(true, LakayaPlayerState->GetCharacterName(),LakayaPlayerState->GetPlayerName());
			}
			
			Players.Remove(LakayaPlayerState);
			break;
		}
	}

	for(const auto PlayerState : Players)
	{
		const auto LakayaPlayerState = Cast<ALakayaBasePlayerState>(PlayerState);
		if(!LakayaPlayerState) return;
		
		if (LakayaPlayerState->GetTeam() == ETeam::Pro)
		{
			UIntroElement* IntroElement = NewObject<UIntroElement>(this, IntroElementProClass);
			ProElements.Emplace(IntroElement);
			ProVerticalBox->AddChildToVerticalBox(IntroElement);
			IntroElement->SetPlayerData(false, LakayaPlayerState->GetCharacterName(), LakayaPlayerState->GetPlayerName());
		}
		else
		{
			UIntroElement* IntroElement = NewObject<UIntroElement>(this, IntroElementAntiClass);
			AntiElements.Emplace(IntroElement);
			AntiVerticalBox->AddChildToVerticalBox(IntroElement);
			IntroElement->SetPlayerData(false, LakayaPlayerState->GetCharacterName(), LakayaPlayerState->GetPlayerName());
		}
	}
}
