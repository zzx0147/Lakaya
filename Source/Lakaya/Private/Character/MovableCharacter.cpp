// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovableCharacter.h"

#include "Character/StatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

AMovableCharacter::AMovableCharacter()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AMovableCharacter::Crouch(bool bClientSimulation)
{
	//TODO: 달리고 있던 경우 앉기가 아닌 슬라이딩이 이뤄지도록 구현합니다.
	Super::Crouch(bClientSimulation);
}

void AMovableCharacter::SetupCharacterServer(const FCharacterSetupData* Data)
{
	Super::SetupCharacterServer(Data);
	if (auto& Stat = GetStatComponent())
	{
		GetCharacterMovement()->MaxWalkSpeed = Stat->GetWalkSpeed();
		Stat->OnWalkSpeedChanged.AddUObject(this, &AMovableCharacter::OnWalkSpeedChanged);
		Stat->OnRunSpeedChanged.AddUObject(this, &AMovableCharacter::OnRunSpeedChanged);
	}
}

void AMovableCharacter::OnRep_StatComponent()
{
	Super::OnRep_StatComponent();
	if (auto& Stat = GetStatComponent())
	{
		GetCharacterMovement()->MaxWalkSpeed = Stat->GetWalkSpeed();
		Stat->OnWalkSpeedChanged.AddUObject(this, &AMovableCharacter::OnWalkSpeedChanged);
		Stat->OnRunSpeedChanged.AddUObject(this, &AMovableCharacter::OnRunSpeedChanged);
	}
}

void AMovableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMovableCharacter, bIsRunning, COND_SkipOwner);
}

void AMovableCharacter::Run()
{
	SetRunStateClient(true);
}

void AMovableCharacter::StopRun()
{
	SetRunStateClient(false);
}

bool AMovableCharacter::SetRunStateServer_Validate(bool IsRunning, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds() + 0.05f >= Time;
}

void AMovableCharacter::SetRunStateServer_Implementation(bool IsRunning, const float& Time)
{
	// 순서가 꼬인 패킷이거나 변경되는 것이 없는 경우 스킵합니다.
	if (RecentRunEventTime > Time || bIsRunning == IsRunning) return;
	RecentRunEventTime = Time;
	bIsRunning = IsRunning;
	ApplySpeedFromStat();
	OnRunStateChanged.Broadcast(bIsRunning);
}

void AMovableCharacter::SetRunStateClient(const bool& IsRunning)
{
	if (bIsRunning == IsRunning) return;
	bIsRunning = IsRunning;
	ApplySpeedFromStat();
	OnRunStateChanged.Broadcast(bIsRunning);
	if (!HasAuthority()) SetRunStateServer(bIsRunning, GetServerTime());
}

void AMovableCharacter::ApplySpeedFromStat()
{
	if (auto& Stat = GetStatComponent())
		GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? Stat->GetRunSpeed() : Stat->GetWalkSpeed();
}

void AMovableCharacter::OnRep_IsRunning()
{
	ApplySpeedFromStat();
}

void AMovableCharacter::OnWalkSpeedChanged(const float& WalkSpeed)
{
	if (!bIsRunning) GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMovableCharacter::OnRunSpeedChanged(const float& RunSpeed)
{
	if (bIsRunning) GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}
