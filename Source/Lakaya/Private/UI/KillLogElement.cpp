// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillLogElement.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

//TODO: 리팩터링 필요
void UKillLogElement::SetKillLog(ACharacter* Attacker, ACharacter* Victim)
{
	AttackerTextBlock->SetText(FText::FromString(Attacker->GetPlayerState()->GetPlayerName()));
	VictimTextBlock->SetText(FText::FromString(Victim->GetPlayerState()->GetPlayerName()));
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

void UKillLogElement::NativeConstruct()
{
	ShowingTime = 5.f;
	InitializeWidgetPtr(AttackerTextBlock, TEXT("Attacker_Text"));
	InitializeWidgetPtr(VictimTextBlock, TEXT("Victim_Text"));
	InitializeWidgetPtr(WeaponImage, TEXT("Weapon_Img"));
}
