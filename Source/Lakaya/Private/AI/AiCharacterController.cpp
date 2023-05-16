// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"
#include "AI/AiCharacter.h"
#include "Character/Ability/AutoFireAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"
#include "GameMode/OccupationGameMode.h"
#include "GameMode/OccupationGameState.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
	bWantsPlayerState = true;
}

void AAiCharacterController::BeginPlay()
{
	Super::BeginPlay();

	AOccupationGameMode* OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
	if (OccupationGameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AiCharacterController_GameMode is null."));
		return;
	}
	
	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_GameState is null."));
		return;
	}

	// OccupationGameState->AddMaxPlayer();
	// OccupationGameState->AddPlayerState(GetCharacter()->GetController()->PlayerState);
	// OccupationGameMode->NumPlayers++;
	// int32 CurrentPlayerNum = OccupationGameState->PlayerArray.Num();
	// OccupationGameState->SetNumPlayers(CurrentPlayerNum);
	// UE_LOG(LogTemp, Warning, TEXT("AiController BeginPlay."));
}

// void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
// {
// 	ArmCharacter = Cast<AArmedCharacter>(GetCharacter());
// 	
// 	auto& Abilities = ArmCharacter->GetAbilities();
// 	if(Abilities.IsValidIndex(WeaponFire))
// 	{
// 		if(const auto FireAbility = Cast<UAutoFireAbility>(Abilities[WeaponFire]))
// 		{
// 			FireAbility->AbilityStart();
// 		}
// 	}	
// }

void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponFire);
}

// void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
// {
// 	ArmCharacter = Cast<AArmedCharacter>(GetCharacter());
// 	
// 	auto& Abilities = ArmCharacter->GetAbilities();
// 	if(Abilities.IsValidIndex(WeaponFire))
// 	{
// 		if(const auto FireAbility = Cast<UAutoFireAbility>(Abilities[WeaponFire]))
// 		{
// 			FireAbility->AbilityStop();
// 		}
// 	}
// }

void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponFire);
}
