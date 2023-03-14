// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableCharacter.h"

#include "CollectorPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "IndividualGameMode.h"

ADamageableCharacter::ADamageableCharacter()
{
	MaximumHealth = 100.f;
}

void ADamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaximumHealth;
	if (IsRunningDedicatedServer()) return;

	OnTakeAnyDamage.AddDynamic(this, &ADamageableCharacter::OnTakeAnyDamageCallback);
	OnKillCharacter.AddDynamic(this, &ADamageableCharacter::OnKillCharacterCallback);
}

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableCharacter, OnKillCharacter);
	DOREPLIFETIME(ADamageableCharacter, MaximumHealth);
	DOREPLIFETIME(ADamageableCharacter, Health);
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	OnKillCharacter.Broadcast(GetController(), this, EventInstigator, DamageCauser);
}

void ADamageableCharacter::OnRep_MaximumHealth()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,
	                                 FString::Printf(TEXT("MaximumHealth Changed : %f"), MaximumHealth));
}

void ADamageableCharacter::OnRep_Health()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Damaged : %f"), Damage));

	// AIndividualGameMode* GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
	// if (!GameMode)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
	// 	return;
	// }
	// else
	// {
	// 	AController* KilledCharacter = nullptr;
	// 	if (DamagedActor->IsA<AController>())
	// 	{
	// 		KilledCharacter = Cast<AController>(DamagedActor);
	// 	}
	// 	else if (DamagedActor->GetInstigator())
	// 	{
	// 		KilledCharacter = DamagedActor->GetInstigator()->GetController();
	// 	}
	//
	// 	if (!KilledCharacter)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("DamageableCharacter_KilledCharacter is null."));
	// 		return;
	// 	}
 //            
	// 	GameMode->OnKilledCharacter(KilledCharacter);
	// }
}

void ADamageableCharacter::OnKillCharacterCallback(AController* KilledController, AActor* KilledActor,
                                                   AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Dead"));

	AIndividualGameMode* GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
		return;
	}
	else
	{
		GameMode->OnKilledCharacter(KilledController);
	}
}