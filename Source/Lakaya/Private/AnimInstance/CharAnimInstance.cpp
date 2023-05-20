// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharAnimInstance.h"


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
}
