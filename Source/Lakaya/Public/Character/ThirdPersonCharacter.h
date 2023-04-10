// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class LAKAYA_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AThirdPersonCharacter();

protected:
	virtual void Tick(float DeltaSeconds) override;

public:
	inline class USpringArmComponent* GetSpringArm() { return SpringArm; }
	inline const class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	inline class UCameraComponent* GetCamera() { return Camera; }
	inline const class UCameraComponent* GetCamera() const { return Camera; }
	inline const bool& GetYawClutch() const { return YawClutch; }
	inline void SetYawClutch(const bool& Enable) { YawClutch = Enable; }

protected:
	bool bFixMeshTransform;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	bool YawClutch;

};
