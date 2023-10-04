// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Occupation/Team.h"
#include "OutlineManager.generated.h"

class ALakayaBaseCharacter;

UCLASS()
class LAKAYA_API AOutlineManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOutlineManager();

	void SetClairvoyance(const bool& bIsClairvoyance);
	void SetTeam(const ETeam& NewTeam);
	void RegisterClairvoyance(const ALakayaBaseCharacter* Character);
	void UnRegisterClairvoyance(const ALakayaBaseCharacter* Character);

	virtual void PostInitializeComponents() override;

private:
	ETeam ClientTeam;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPostProcessComponent> ClairvoyancePostProcessComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ClairvoyanceMaterial;

	UPROPERTY(EditAnywhere)
	FName EnemyRenderingParameterName;

	UPROPERTY(EditAnywhere)
	FName EnemyNumberStartParameterName;

	TSet<const ALakayaBaseCharacter*> ActivatedClairvoyanceSet;
	TWeakObjectPtr<UMaterialInstanceDynamic> ClairvoyanceDynamic;
};
