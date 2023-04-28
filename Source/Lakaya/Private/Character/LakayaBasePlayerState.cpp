// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBasePlayerState.h"

#include "Character/LakayaBaseCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void ALakayaBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBasePlayerState, Health);
	DOREPLIFETIME(ALakayaBasePlayerState, Team);
	DOREPLIFETIME(ALakayaBasePlayerState, RespawnTime);
}

void ALakayaBasePlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	OnPawnSet.AddUniqueDynamic(this, &ALakayaBasePlayerState::OnPawnSetCallback);
	bRecentAliveState = true;
}

float ALakayaBasePlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                         AController* EventInstigator, AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage == 0.f) return 0.f;
	Health -= Damage;

	// 힐이고, 최대체력을 초과한 경우 최대체력으로 맞춰줍니다.
	if (Damage < 0.f)
		if (const auto MaxHealth = GetMaxHealth(); Health > MaxHealth)
			Health = MaxHealth;

	OnHealthChanged.Broadcast(Health);
	NoticePlayerHit(*DamageCauser->GetName(), DamageCauser->GetActorLocation(), Damage);
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

bool ALakayaBasePlayerState::IsAlive() const
{
	// 부활시간이 설정되지 않았거나(0.f), 부활시간이 지난 경우 생존으로 간주합니다.
	// 반대로 부활시간이 음수이거나, 아직 부활시간을 지나지 않은 경우 사망으로 간주합니다.
	return RespawnTime >= 0.f && RespawnTime < GetServerTime();
}

void ALakayaBasePlayerState::MakeAlive()
{
	RespawnTime = 0.f;
	BroadcastWhenAliveStateChanged();
}

float ALakayaBasePlayerState::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

bool ALakayaBasePlayerState::ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                              AController* EventInstigator, AActor* DamageCauser)
{
	// 플레이어가 생존해있고, 데미지가 공격이 아닌 회복인 경우나 EventInstigator가 nullptr인 경우, 같은 팀이 아닌 경우 피해를 받도록 합니다.
	return IsAlive() && (DamageAmount > 0.f || !EventInstigator
		|| !IsSameTeam(EventInstigator->GetPlayerState<ALakayaBasePlayerState>()));
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

void ALakayaBasePlayerState::OnRep_RespawnTime()
{
	BroadcastWhenAliveStateChanged();
}

void ALakayaBasePlayerState::BroadcastWhenAliveStateChanged()
{
	const auto AliveState = IsAlive();

	// 생존 상태가 변경된 것이 없는 경우 아무 것도 하지 않습니다.
	if (bRecentAliveState == AliveState) return;

	OnAliveStateChanged.Broadcast(AliveState);
	bRecentAliveState = AliveState;
}

void ALakayaBasePlayerState::NoticePlayerHit_Implementation(const FName& CauserName, const FVector& CauserLocation,
                                                            const float& Damage)
{
	//TODO: 피격 레이더를 업데이트 합니다.
}
