// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillLogElement.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

//TODO: 리팩터링 필요
void UKillLogElement::SetKillLog(APlayerState* Attacker, APlayerState* Victim)
{
	if (const auto BaseAttacker = Cast<ALakayaBasePlayerState>(Attacker))
	{
		if(AttackerTextBlock.IsValid())
			AttackerTextBlock->SetText(FText::FromString(Attacker->GetPlayerName()));

		if (AttackerImage.IsValid() && CharacterImageMap.Contains(BaseAttacker->GetCharacterName()))
		{
			AttackerImage->SetBrushFromTexture(CharacterImageMap[BaseAttacker->GetCharacterName()]);
		}

		if(WeaponImage.IsValid() && WeaponImageMap.Contains(BaseAttacker->GetCharacterName()))
		{
			WeaponImage->SetBrushFromTexture(WeaponImageMap[BaseAttacker->GetCharacterName()]);
		}

		if(BackgroundImageMap.Contains(BaseAttacker->GetTeam()))
		{
			BackgroundImage->SetBrushFromTexture(BackgroundImageMap[BaseAttacker->GetTeam()]);
		}
	}
	
	if (const auto BaseVictim = Cast<ALakayaBasePlayerState>(Victim))
	{
		if(VictimTextBlock.IsValid())
			VictimTextBlock->SetText(FText::FromString(Victim->GetPlayerName()));

		if (VictimImage.IsValid() && CharacterImageMap.Contains(BaseVictim->GetCharacterName()))
		{
			VictimImage->SetBrushFromTexture(CharacterImageMap[BaseVictim->GetCharacterName()]);
		}
	}

	
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(ShowTimer, this, &UKillLogElement::ExpireKillLog, ShowingTime);
}

void UKillLogElement::ExpireInstant()
{
	GetWorld()->GetTimerManager().ClearTimer(ShowTimer);
	ExpireKillLog();
}

void UKillLogElement::ExpireKillLog()
{
	SetVisibility(ESlateVisibility::Collapsed);
	Return(this);
}

UKillLogElement::UKillLogElement(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	WeaponImageMap.Emplace(FName(TEXT("Rena")));
	WeaponImageMap.Emplace(FName(TEXT("Wazi")));
	WeaponImageMap.Emplace(FName(TEXT("Minami")));

	CharacterImageMap.Emplace(FName(TEXT("Rena")));
	CharacterImageMap.Emplace(FName(TEXT("Wazi")));
	CharacterImageMap.Emplace(FName(TEXT("Minami")));

	BackgroundImageMap.Emplace(EPlayerTeam::A);
	BackgroundImageMap.Emplace(EPlayerTeam::B);
	BackgroundImageMap.Emplace(EPlayerTeam::Individual);
	
}

void UKillLogElement::NativeConstruct()
{
	ShowingTime = 5.f;
	InitializeWidgetPtr(AttackerTextBlock, TEXT("Attacker_Text"));
	InitializeWidgetPtr(VictimTextBlock, TEXT("Victim_Text"));
	InitializeWidgetPtr(WeaponImage, TEXT("Weapon_Img"));
	InitializeWidgetPtr(AttackerImage, TEXT("Attacker_Img"));
	InitializeWidgetPtr(VictimImage, TEXT("Victim_Img"));
	InitializeWidgetPtr(BackgroundImage, TEXT("Background_Img"));
	
}
