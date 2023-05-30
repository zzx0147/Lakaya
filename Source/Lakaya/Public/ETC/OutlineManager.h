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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetClairvoyance(const bool& bIsClairvoyance) const;

public:
	void SetTeam(const EPlayerTeam& NewTeam) { ClientTeam = NewTeam; }
	void RegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam);
	void UnRegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam);

private:
	EPlayerTeam ClientTeam;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPostProcessComponent> OutlinePostProcessComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPostProcessComponent> ClairvoyancePostProcessComponent;

	TSet<uint32> ActivatedClairvoyanceSet;
};
