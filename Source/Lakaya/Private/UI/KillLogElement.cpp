// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KillLogElement.h"

#include "AIController.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/AIIndividualGameState.h"

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

		// if(BackgroundImageMap.Contains(BaseAttacker->GetTeam()))
		// {
		// 	BackgroundImage->SetBrushFromTexture(BackgroundImageMap[BaseAttacker->GetTeam()]);
		// }

		// TODO : AI 개인전 GameState 일때는 공격자의 킬로그 이미지를 변경하고 아닐땐 공격자의 팀의 따른 이미지를 표시하게 구현하였습니다.
		if(BackgroundImage.IsValid())
		{
			ALakayaBaseGameState* NowBaseGameState =
				Cast<ALakayaBaseGameState>(BaseAttacker->GetWorld()->GetGameState());
			
			if(NowBaseGameState)
			{
				AAIIndividualGameState* AIIndividualGameState =
					Cast<AAIIndividualGameState>(NowBaseGameState);
				
				if(AIIndividualGameState)
				{
					for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
					{
						AController* AllControllers = It->Get();

						if(AllControllers == BaseAttacker->GetPawn()->GetController())
						{
							if(APlayerController* PlayerAttackerController =
								Cast<APlayerController>(AllControllers))
							{
								if(PlayerAttackerController)
								{
									//임시 비활성화
									// UTexture2D* IndividualPlayerKillLogImage =
									// 	LoadObject<UTexture2D>(nullptr,
									// 		TEXT("Texture2D'/Game/UI/individual/ingame_scoreboard/T_Individual_In_killlogbar_player.T_Individual_In_killlogbar_player'"));
									// BackgroundImage->SetBrushFromTexture(IndividualPlayerKillLogImage);
								}
							}
							else if(AAIController* AIAttackerController =
								Cast<AAIController>(AllControllers))
							{
								if(AIAttackerController)
								{
									// UTexture2D* IndividualPlayerKillLogImage =
									// 	LoadObject<UTexture2D>(nullptr,
									// 		TEXT("Texture2D'/Game/UI/individual/ingame_scoreboard/T_Individual_In_killlogbar.T_Individual_In_killlogbar'"));
									// BackgroundImage->SetBrushFromTexture(IndividualPlayerKillLogImage);
								}
							}
						}
						else
						{
							if(BackgroundImageMap.Contains(BaseAttacker->GetTeam()))
								BackgroundImage->SetBrushFromTexture(BackgroundImageMap[BaseAttacker->GetTeam()]);
						}
					}
				}
				else
				{
					if(BackgroundImageMap.Contains(BaseAttacker->GetTeam()))
						BackgroundImage->SetBrushFromTexture(BackgroundImageMap[BaseAttacker->GetTeam()]);
				}
			}
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
	WeaponImageMap.Emplace(FName(TEXT("Gangrim")));

	CharacterImageMap.Emplace(FName(TEXT("Rena")));
	CharacterImageMap.Emplace(FName(TEXT("Wazi")));
	CharacterImageMap.Emplace(FName(TEXT("Gangrim")));

	BackgroundImageMap.Emplace(ETeam::Anti);
	BackgroundImageMap.Emplace(ETeam::Pro);
	BackgroundImageMap.Emplace(ETeam::Individual);
	
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
