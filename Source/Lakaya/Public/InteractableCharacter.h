// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArmedCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "UObject/WeakInterfacePtr.h"
#include "InteractableCharacter.generated.h"

DECLARE_EVENT_TwoParams(AInteractableCharacter, FInteractionStartSignature, const float&, AActor*);

DECLARE_EVENT_TwoParams(AInteractableCharacter, FInteractionEndedSignature, const float&, const bool&);

/**
 * 
 */
UCLASS()
class LAKAYA_API AInteractableCharacter : public AArmedCharacter
{
	GENERATED_BODY()

public:
	AInteractableCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

public:
	/**
	 * @brief 이 캐릭터가 인터렉션을 시작함을 서버와 모든 클라이언트에게 알립니다.
	 * @param Time 인터렉션이 시작된 시간입니다. 보통은 캐릭터가 서버에 인터렉션 시작을 요청했을 때 호출됩니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void InteractionStartNotify(const float& Time, AActor* Actor);

	/**
	 * @brief 이 캐릭터가 인터렉션을 종료했음을 서버와 모든 클라이언트에게 알립니다.
	 * @param Time 인터렉션이 종료되는 시간입니다.
	 * @param IsSuccess 인터렉션 성공 여부를 나타냅니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void InteractionEndedNotify(const float& Time, bool IsSuccess);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStop(const float& Time, AActor* Actor);

	// 현재시점의 서버 시간을 가져옵니다.
	inline float GetServerTime() { return GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); }

private:
	void InteractionStart(const FInputActionValue& Value);
	void InteractionStop(const FInputActionValue& Value);

public:
	/**
	 * @brief 인터렉션이 시작되었을 때 호출됩니다. 인터렉션 시작 시간, 대상 액터를 매개변수로 받습니다.
	 * 이 이벤트는 오래 걸리는 상호작용시를 상정하여 선언되었으므로 즉시 실행되는 상호작용의 경우 호출되지 않을 수 있습니다.
	 */
	FInteractionStartSignature OnInteractionStarted;

	/**
	 * @brief 인터렉션이 끝났을 때 호출됩니다. 끝나는 시간, 성공 여부를 매개변수로 받습니다.
	 */
	FInteractionEndedSignature OnInteractionEnded;

private:
	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	UInputMappingContext* InteractionContext;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	int8 InteractionPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStartAction;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStopAction;

	UPROPERTY(EditAnywhere, Category = Interaction)
	float InteractionRange;

	UPROPERTY(EditAnywhere, Category = Interaction)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	uint8 InteractableCount;
	TWeakObjectPtr<AActor> InteractingActor;
	FCollisionQueryParams TraceQueryParams;
};
