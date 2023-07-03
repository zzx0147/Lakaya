// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableCharacter.h"
#include "WaziCharacter.generated.h"

UCLASS()
class LAKAYA_API AWaziCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	explicit AWaziCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool ShouldStartAbility_Implementation(EAbilityKind Kind) override;
	virtual bool ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind) override;
	virtual bool ShouldInteract() const override;

	virtual void OnOverdriveStartTimeNotified(const float& Time);
	virtual void OnClairvoyanceStartTimeNotified(const float& Time);
	virtual void OnWantsToFireChanged(bool FireState);
	virtual void OnSmokePerformTimeNotified(const float& Time);
	virtual void OnReloadStateChanged(bool ReloadState);
	virtual void OnReloadCompleteTimeNotified(const float& Time);

public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

private:
	void CreateClairvoyanceWidget(APlayerController* PlayerController);

protected:
	// 오버드라이브 스킬 사용 이후 적용될 후딜레이 시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float OverdriveLateDelay;

	// 투시 스킬 사용 이후 적용될 후딜레이 시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float ClairvoyanceLateDelay;

	// 연막탄 투척 이후 후딜레이 시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float SmokeLateDelay;

private:
	float OverdriveEndingTime;
	float ClairvoyanceEndingTime;
	bool bWantsToFire;
	float SmokeEndingTime;
	bool bIsReloading;
	float ReloadCompleteTime;
};
