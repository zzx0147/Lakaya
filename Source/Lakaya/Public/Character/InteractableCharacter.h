// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArmedCharacter.h"
#include "InteractableCharacter.generated.h"

DECLARE_EVENT_OneParam(AInteractableCharacter, FInteractionSignature, AActor*);

/**
 *
 */
UCLASS(Config = Game)
class LAKAYA_API AInteractableCharacter : public AArmedCharacter
{
	GENERATED_BODY()

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;

public:
	// 주변에 인터렉션 가능한 액터가 존재하는 경우 인터렉션을 시작합니다.
	// void StartInteraction();
	
	// 현재 인터렉션 중인 액터가 있는 경우 인터렉션을 중단합니다.
	// void StopInteraction();

	/**
	 * @brief 현재 인터렉션이 가능한지 판별합니다.
	 */
	bool ShouldInteract();
	
	// 인터렉션을 초기화합니다. 인터렉션이 끝나는 경우 호출됩니다.
	void InitializeInteraction();

	// AActor* GetInteractableActor() const { return InteractableActor.Get(); }
	// AActor* GetInteractingActor() const { return InteractingActor.Get(); }

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStop(const float& Time, AActor* Actor);

	UFUNCTION()
	void OnRep_InteractingActor();

public:
	// 인터렉션이 가능한 액터가 변경되는 경우 호출됩니다. 매개변수로 넘겨진 액터가 nullptr이면 인터렉션이 불가능해졌음을 의미하며,
	// 그렇지 않은 경우 해당 액터와 인터렉션이 가능해졌음을 의미합니다.
	FInteractionSignature OnInteractableActorChanged;

	// 인터렉션중인 액터가 변화한 경우 호출됩니다. 매개변수로 넘겨진 액터가 nullptr이면 인터렉션 중단을 의미하며, 그렇지 않은 경우 인터렉션중임을 의미합니다.
	FInteractionSignature OnInteractingActorChanged;

private:
	// 현재 인터렉션 중인 액터입니다.
	UPROPERTY(ReplicatedUsing=OnRep_InteractingActor)
	TWeakObjectPtr<AActor> InteractingActor;

	// 인터렉션이 가능한 액터입니다.
	TWeakObjectPtr<AActor> InteractableActor;

	// UPROPERTY()
};