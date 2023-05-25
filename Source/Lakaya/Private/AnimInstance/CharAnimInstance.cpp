// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharAnimInstance.h"

#include "Character/Ability/CoolTimedSummonAbility.h"
#include "GameFramework/GameStateBase.h"


UCharAnimInstance::UCharAnimInstance()
{
	FireAnimDuration = 0.1f;
}

void UCharAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if(const auto Character = Cast<AArmedCharacter>(TryGetPawnOwner()))
	{
		if(const auto ResultNotifyFireAbility = Character->FindAbility<UResultNotifyFireAbility>(WeaponFire))
		{
			ResultNotifyFireAbility->OnSingleFire.
			AddLambda([this](const FVector&,const FVector&, const FVector&, const EFireResult&)
			{
				RecentFireTime = GetWorld()->TimeSeconds; 
			} );
			UE_LOG(LogTemp, Error, TEXT("Fire!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
		}

		if(const auto ReloadAbility = Character->FindAbility<UReloadAbility>(WeaponReload))
		{
			ReloadAbility->OnReloadStateChanged.
			AddLambda([this](const bool& ReloadState)
				{bIsReload = ReloadState;} );
		}

		if (const auto Ability = Character->FindAbility<UCoolTimedSummonAbility>(WeaponAbility))
		{
			Ability->OnPerformTimeNotified.AddLambda([this](const float& ActualFireTime)
			{
				RecentWeaponSkillTime = ActualFireTime;
			});
		}

		if (const auto InteractableCharacter = Cast<AInteractableCharacter>(Character))
		{
			InteractableCharacter->OnInteractingActorChanged.
			AddUObject(this, &UCharAnimInstance::OnInteractingActorChanged);
		}
	}
}

void UCharAnimInstance::OnInteractingActorChanged(AActor* NewInteractingActor)
{
	if (const auto InteractableCharacter =
		Cast<AInteractableCharacter>(TryGetPawnOwner()))
	{
		bIsInteracting = NewInteractingActor != nullptr;

		// if (NewInteractingActor)
		// {
		// 	bIsInteracting = true;
		// }
		// else
		// {
		// 	bIsInteracting = false;
		// }
	}
}

void UCharAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	bIsAutoFire = RecentFireTime + FireAnimDuration > GetWorld()->TimeSeconds;

	const auto GameStateBase = GetWorld()->GetGameState();
	const float CurrentTime = GameStateBase ? GameStateBase->GetServerWorldTimeSeconds() : GetWorld()->TimeSeconds;
	bIsWeaponSkill = RecentWeaponSkillTime <= CurrentTime
		&& RecentWeaponSkillTime + WeaponSkillAnimDuration > CurrentTime;
}
