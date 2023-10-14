// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BulletSpreadComponent.h"

#include <algorithm>

#include "Character/LakayaBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBulletSpreadComponent::UBulletSpreadComponent(): BulletSpreadAmountByMovement(0), BulletSpreadAmountByFire(0), bIsZoomed(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UBulletSpreadComponent::BeginPlay()
{
	Super::BeginPlay();
	LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(GetOwner());
	if (LakayaBaseCharacter.IsValid())
	{
		CharacterMovementComponent = LakayaBaseCharacter->GetCharacterMovement();
		if(const auto& AbilitySystem = LakayaBaseCharacter->GetAbilitySystemComponent())
		{
			AbilitySystem->RegisterGameplayTagEvent(ZoomTag,EGameplayTagEventType::NewOrRemoved).AddWeakLambda(this,[&](const FGameplayTag Tag,int32 Count)
			{
				bIsZoomed = Count >= 1;
			});
		}
	}
}

void UBulletSpreadComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	BulletSpreadAmountByFire -= DecreaseSpreadAmountPerSec * DeltaTime;
	BulletSpreadAmountByFire = FMath::Clamp(BulletSpreadAmountByFire, 0.0f, MaxSpreadAmountByFire);
	
	if (!CharacterMovementComponent.IsValid()) return;

	BulletSpreadAmountByMovement = 0.0f;
	BulletSpreadAmountByMovement += CharacterMovementComponent->Velocity.Length() * CharacterVelocityMultiplier;
	if (CharacterMovementComponent->IsFalling()) BulletSpreadAmountByMovement += SpreadAmountOnFalling;
	else if (CharacterMovementComponent->IsCrouching()) BulletSpreadAmountByMovement += SpreadAmountOnCrouch;
	if(bIsZoomed) BulletSpreadAmountByMovement += SpreadAmountOnZoom;

	OnChangeBulletSpreadAmountSignature.Broadcast(GetTotalSpreadAmount());
}

void UBulletSpreadComponent::AddSpreadAmount(const float& AdditionalAmount)
{
	BulletSpreadAmountByFire += AdditionalAmount;
	BulletSpreadAmountByFire = FMath::Clamp(BulletSpreadAmountByFire, 0.0f, MaxSpreadAmountByFire);
}
