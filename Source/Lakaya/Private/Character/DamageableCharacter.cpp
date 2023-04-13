// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DamageableCharacter.h"

#include "Character/StatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

//TODO: 아래 주석의 구현 부분은 추후 OccupationCharacter로 옮겨져야 합니다.
// bool ADamageableCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
//                                             AActor* DamageCauser) const
// {
// 	 auto MyPlayerState = GetPlayerState();
// 	 if (MyPlayerState == nullptr)
// 	 {
// 	 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No Player State"));
// 	 	return false;
// 	 }
// 	 auto CollectorPlayerState = Cast<ACollectorPlayerState>(MyPlayerState);
// 	 if (CollectorPlayerState == nullptr)
// 	 {
// 	 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No CollectorPlayer State"));
// 	 	return false;
// 	 }
// 	
// 	 if (EventInstigator == nullptr) return 0.0f;
// 	
// 	 auto enemyPlayerState = EventInstigator->GetPlayerState<ACollectorPlayerState>();
// 	 if (enemyPlayerState == nullptr)
// 	 {
// 	 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No EnemyPlayer State"));
// 	 	return false;
// 	 }
// 	 if (enemyPlayerState->GetPlayerTeamState() == CollectorPlayerState->GetPlayerTeamState())
// 	 {
// 	 	return false;
// 	 }
// 	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
// }

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;

	// 힐이 최대체력 이상으로 이뤄지는 경우 최대체력으로 맞춰줍니다.
	if (Damage < 0.f)
		if (auto& Stat = GetStatComponent())
			if (Health > Stat->GetMaximumHealth())
				Health = Stat->GetMaximumHealth();

	OnHealthChanged.Broadcast(Health);
	NotifyDamageReceive(DamageCauser->GetActorLocation(), Damage);

	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::SetupCharacterServer(const FCharacterSetupData* Data)
{
	Super::SetupCharacterServer(Data);

	bIsAlive = true;
	OnAliveChanged.Broadcast(bIsAlive);

	if (auto& Stat = GetStatComponent())
	{
		Health = Stat->GetMaximumHealth();
		OnHealthChanged.Broadcast(Health);
	}
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableCharacter, Health);
	DOREPLIFETIME(ADamageableCharacter, bIsAlive);
}

void ADamageableCharacter::Respawn()
{
	bIsAlive = true;
	OnAliveChanged.Broadcast(bIsAlive);

	if (auto& Stat = GetStatComponent())
	{
		Health = Stat->GetMaximumHealth();
		OnHealthChanged.Broadcast(Health);
	}

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	bIsAlive = false;
	OnAliveChanged.Broadcast(bIsAlive);
	GetCharacterMovement()->DisableMovement();
	//TODO: 게임모드 또는 게임스테이트를 가져와서 함수를 호출하여 이 캐릭터가 다른 캐릭터에 의해 살해당했음을 알려줍니다.
}

void ADamageableCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnRep_IsAlive()
{
	OnAliveChanged.Broadcast(bIsAlive);
}

void ADamageableCharacter::NotifyDamageReceive_Implementation(const FVector& AttackLocation, const float& Damage)
{
	OnDamageReceived.Broadcast(AttackLocation, Damage);
}
