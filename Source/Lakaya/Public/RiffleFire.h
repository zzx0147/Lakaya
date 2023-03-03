// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "Components/ActorComponent.h"
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
class LAKAYA_API URiffleFire : public UActorComponent, public IWeaponFire
{
	GENERATED_BODY()

public:
	URiffleFire();

protected:
	virtual void BeginPlay() override;

private:
	virtual void SetupData_Implementation(const FName& RowName) override;
	void FireStart_Implementation(const float& Time);
	void FireStop_Implementation(const float& Time);
	void SwitchSelector_Implementation(const float& Time);
	void FireStartNotify_Implementation(const float& Time);
	void FireStopNotify_Implementation(const float& Time);

	float LockstepTimerTime(const float& Time) const;
	void TraceFire();
	void StopFire();
	void UpdateFireMode();

	UPROPERTY(EditAnywhere, Category=DataTable)
	class UDataTable* WeaponFireDataTable;

	EFireMode FireMode;
	EFireMode DesiredFireMode;
	uint8 FireCount;
	FTimerHandle StartTimer;
	FTimerHandle StopTimer;
	FTimerHandle SwitchModeTimer;
	TWeakObjectPtr<class AThirdPersonCharacter> Character;
	FCollisionQueryParams TraceQueryParams;
};
