// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBasePlayerState.h"

#include "Character/LakayaBaseCharacter.h"
#include "Net/UnrealNetwork.h"

void ALakayaBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBasePlayerState, Health);
	DOREPLIFETIME(ALakayaBasePlayerState, Team);
}

void ALakayaBasePlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	OnPawnSet.AddUniqueDynamic(this, &ALakayaBasePlayerState::OnPawnSetCallback);
}

float ALakayaBasePlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                         AController* EventInstigator, AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Health -= Damage;
	OnHealthChanged.Broadcast(Health);
	if (Health < 0.f) OnPlayerKilled.Broadcast(GetOwningController(), DamageCauser, EventInstigator);

	return Damage;
}

void ALakayaBasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (const auto Other = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		Other->Health = Health;
		Other->Team = Team;
	}
}

bool ALakayaBasePlayerState::IsSameTeam(const ALakayaBasePlayerState* Other) const
{
	// 두 플레이어가 개인전상태가 아니고, Team 값이 같은 경우 같은 팀으로 판별합니다.
	return Other && Other->Team != EPlayerTeam::Individual && Team != EPlayerTeam::Individual && Other->Team == Team;
}

void ALakayaBasePlayerState::SetTeam(const EPlayerTeam& DesireTeam)
{
	Team = DesireTeam;
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->OnSetTeam(Team);
	OnTeamChanged.Broadcast(Team);
}

bool ALakayaBasePlayerState::ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                              AController* EventInstigator, AActor* DamageCauser)
{
	// 피해가 아닌 힐인 경우나 EventInstigator가 nullptr인 경우, 같은 팀이 아닌 경우 피해를 받도록 합니다.
	return DamageAmount > 0.f || !EventInstigator
		|| !IsSameTeam(EventInstigator->GetPlayerState<ALakayaBasePlayerState>());
}

void ALakayaBasePlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (Team != EPlayerTeam::None)
		if (const auto Character = Cast<ALakayaBaseCharacter>(NewPawn))
			Character->OnSetTeam(Team);

	if (HasAuthority())
	{
		// 캐릭터가 변경된 경우 그 캐릭터에 맞는 체력으로 재설정합니다.
		Health = GetMaxHealth();
		OnHealthChanged.Broadcast(Health);
	}
}

float ALakayaBasePlayerState::GetMaxHealth() const
{
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) return Character->GetCharacterMaxHealth();
	return 0.f;
}

void ALakayaBasePlayerState::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health);
}

void ALakayaBasePlayerState::OnRep_Team()
{
	if (const auto Character = GetPawn<ALakayaBaseCharacter>()) Character->OnSetTeam(Team);
	OnTeamChanged.Broadcast(Team);
}
