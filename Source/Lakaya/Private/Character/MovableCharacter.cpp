// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovableCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

AMovableCharacter::AMovableCharacter()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	RunMultiplier = 1.3f;
}

void AMovableCharacter::Crouch(bool bClientSimulation)
{
	//TODO: 달리고 있던 경우 앉기가 아닌 슬라이딩이 이뤄지도록 구현합니다.
	Super::Crouch(bClientSimulation);
}

void AMovableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMovableCharacter, bIsRunning, COND_SkipOwner);
}

bool AMovableCharacter::IsOwnedByLocalPlayer() const
{
	const auto PlayerController = Cast<APlayerController>(GetController());
	return PlayerController && PlayerController->IsLocalController();
}

void AMovableCharacter::Run()
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
	RequestSetRunState(bIsRunning, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

void AMovableCharacter::StopRun()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
	RequestSetRunState(bIsRunning, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

bool AMovableCharacter::RequestSetRunState_Validate(bool IsRunning, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds() + 0.05f >= Time;
}

void AMovableCharacter::RequestSetRunState_Implementation(bool IsRunning, const float& Time)
{
	// 순서가 꼬인 패킷이거나 변경되는 것이 없는 경우 스킵합니다.
	if (RecentRunEventTime > Time || bIsRunning == IsRunning) return;

	RecentRunEventTime = Time;
	bIsRunning = IsRunning;
	if (bIsRunning) GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
	else GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}

void AMovableCharacter::OnRep_IsRunning()
{
	if (bIsRunning) GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
	else GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}
