// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayerController.h"

#include "Character/DamageableCharacter.h"
#include "UI/GamePlayKillLogWidget.h"


ABattlePlayerController::ABattlePlayerController()
{
	static const ConstructorHelpers::FClassFinder<UGamePlayKillLogWidget> KillLogFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayKillLogWidget"));

	KillLogClass = KillLogFinder.Class;
	if (!KillLogClass) UE_LOG(LogController, Fatal, TEXT("Fail to find KillLogClass!"));
}

void ABattlePlayerController::OnCharacterBeginPlay(ACharacter* Character)
{
	if (auto Damageable = Cast<ADamageableCharacter>(Character))
	{
		if (IsLocalController())
		{
			if (!KillLogWidget)
			{
				KillLogWidget = CreateWidget<UGamePlayKillLogWidget>(this, KillLogClass);
				KillLogWidget->AddToViewport();
			}

			KillLogWidget->UpdateKillLogWidget(Cast<ADamageableCharacter>(Character));
		}
	}
}
