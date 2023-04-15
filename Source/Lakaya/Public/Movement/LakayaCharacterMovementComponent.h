#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LakayaCharacterMovementComponent.generated.h"

UCLASS()
class LAKAYA_API ULakayaCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Lakaya : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToSprint : 1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData);
		virtual void PrepMoveFor(ACharacter* C) override;

	};

	class FNetworkPredictionData_Client_Lakaya : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Lakaya(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;

	};
	

	UPROPERTY(EditDefaultsOnly)
	float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float Walk_MaxWalkSpeed;


	bool Safe_bWantsToSprint;

public:
	ULakayaCharacterMovementComponent();
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	UFUNCTION(Blueprintcallable)
	void SprintPressed();

	UFUNCTION(Blueprintcallable)
	void SprintReleased();

	UFUNCTION(Blueprintcallable)
	void CrouchPressed();

};
