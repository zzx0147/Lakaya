 #include "Movement/LakayaCharacterMovementComponent.h"
#include "GameFramework/Character.h"

#pragma region SavedMove



bool ULakayaCharacterMovementComponent::FSavedMove_Lakaya::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Lakaya* NewLakayaMove = static_cast<FSavedMove_Lakaya*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewLakayaMove->Saved_bWantsToSprint)
	{
		return false;
	}


	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void ULakayaCharacterMovementComponent::FSavedMove_Lakaya::Clear()
{
	Super::Clear();

	Saved_bWantsToSprint = 0;
}

uint8 ULakayaCharacterMovementComponent::FSavedMove_Lakaya::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint) Result |= FLAG_Custom_0;

	return Result;
}

void ULakayaCharacterMovementComponent::FSavedMove_Lakaya::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	ULakayaCharacterMovementComponent* CharacterMovement = Cast<ULakayaCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void ULakayaCharacterMovementComponent::FSavedMove_Lakaya::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	ULakayaCharacterMovementComponent* CharacterMovement = Cast<ULakayaCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}
#pragma endregion

#pragma region ClientNetworkPredictionData


ULakayaCharacterMovementComponent::FNetworkPredictionData_Client_Lakaya::FNetworkPredictionData_Client_Lakaya(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr ULakayaCharacterMovementComponent::FNetworkPredictionData_Client_Lakaya::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Lakaya()); 
}

FNetworkPredictionData_Client* ULakayaCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);
	
	if (ClientPredictionData == nullptr)
	{
		ULakayaCharacterMovementComponent* MutableThis = const_cast<ULakayaCharacterMovementComponent*>(this);
		
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Lakaya(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.0f;
	}

	return ClientPredictionData;
}
#pragma endregion

#pragma region CharacterMovementComponent

ULakayaCharacterMovementComponent::ULakayaCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true; 
}

void ULakayaCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void ULakayaCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
}
#pragma endregion

#pragma region Input
void ULakayaCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void ULakayaCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void ULakayaCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = !bWantsToCrouch;
}
#pragma endregion