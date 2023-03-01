// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "GameFramework/GameStateBase.h"
#include "RiffleFire.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Semi,
	Burst,
	Auto
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API URiffleFire : public UWeaponComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	bool FireStart_Validate(const float& RequestTime, const FString& Arg) { return true; }
	void FireStart_Implementation(const float& Time, const FString& Arg);
	void FireStartNotify_Implementation(const float& Time, const FString& Arg);
	bool FireStop_Validate(const float& RequestTime, const FString& Arg) { return true; }
	void FireStop_Implementation(const float& Time, const FString& Arg);
	void FireStopNotify_Implementation(const float& Time, const FString& Arg);
	void SwitchSelector_Implementation(const float& Time, const FString& Arg);

	float LockstepTimerTime(const float& Time) const;
	void TraceFire();
	void StopFire();
	void UpdateFireMode();

	EFireMode FireMode;
	EFireMode DesiredFireMode;
	uint8 FireCount;
	FTimerHandle StartTimer;
	FTimerHandle StopTimer;
	FTimerHandle SwitchModeTimer;
	TWeakObjectPtr<class AThirdPersonCharacter> Character;
	FCollisionQueryParams TraceQueryParams;
};
