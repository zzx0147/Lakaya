// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Occupation/PlayerTeam.h"
#include "OutlineManager.generated.h"

UCLASS()
class LAKAYA_API AOutlineManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOutlineManager();
	virtual void PostInitializeComponents() override;

protected:
	void SetClairvoyance(const bool& bIsClairvoyance);

public:
	void SetTeam(const EPlayerTeam& NewTeam) { ClientTeam = NewTeam; }
	void RegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam);
	void UnRegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam);

private:
	EPlayerTeam ClientTeam;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPostProcessComponent> ClairvoyancePostProcessComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ClairvoyanceMaterial;

	UPROPERTY(EditAnywhere)
	FName EnemyRenderingParameterName;

	TSet<uint32> ActivatedClairvoyanceSet;
	TWeakObjectPtr<UMaterialInstanceDynamic> ClairvoyanceDynamic;
};
