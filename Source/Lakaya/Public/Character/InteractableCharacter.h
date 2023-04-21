// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArmedCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "InputActionValue.h"
#include "InteractableCharacter.generated.h"

DECLARE_EVENT_ThreeParams(AInteractableCharacter, FInteractionStartSignature, const float&, AActor*, const float&);

DECLARE_EVENT_OneParam(AInteractableCharacter, FInteractionEndedSignature, const float&);

/**
 *
 */
UCLASS(Config = Game)
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
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;

public:
	/**
	 * @brief 인터렉션 시작 절차를 시작합니다. 반드시 서버에서 호출해야 합니다.
	 * @param Time 인터렉션 시작 입력이 시작된 시간입니다.
	 * @param Actor 인터렉션을 시작하는 대상 액터입니다.
	 * @param Duration 인터렉션이 수행되는 시간입니다. 시간이 긴 경우 모든 클라이언트에게 전파됩니다.
	 */
	void InitiateInteractionStart(const float& Time, AActor* Actor, const float& Duration = -1.f);

	/**
	 * @brief 즉시 발생하는 인터렉션임을 오너 클라이언트에게 알립니다.
	 */
	void NoticeInstantInteractionLocal();

	/**
	 * @brief 인터렉션 중단 절차를 시작합니다. 반드시 서버에서 호출해야 합니다.
	 * @param Time 인터렉션 중단 입력이 시작된 시간입니다.
	 * @param Actor 인터렉션 중단 대상 액터입니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void InteractionStopNotify(const float& Time, AActor* Actor);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStop(const float& Time, AActor* Actor);

	// 현재시점의 서버 시간을 가져옵니다.
	FORCEINLINE float GetServerTime() const { return GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); }

private:
	/**
	 * @brief 이 캐릭터가 인터렉션을 시작함을 서버와 모든 클라이언트에게 알립니다. 오랫동안 실행되는 인터렉션일 때 사용됩니다.
	 * @param Time 인터렉션이 시작된 시간입니다. 보통은 캐릭터가 서버에 인터렉션 시작을 요청했을 때 호출됩니다.
	 * @param Actor 인터렉션을 시작하는 대상 액터입니다.
	 * @param Duration 인터렉션의 지속 시간입니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void InteractionStartNotify(const float& Time, AActor* Actor, const float& Duration);

	/**
	 * @brief 락스텝 이벤트를 예약합니다. 너무 늦게 예약된 이벤트인 경우 로그를 남기고 취소됩니다.
	 * @param Time 락스텝 이벤트가 요청된 시간입니다.
	 * @param Callback 락스텝 이벤트가 목표로 하는 시간에 실행할 함수입니다.
	 */
	void InitiateLockstepEvent(const float& Time, std::function<void()> Callback);

	void InteractionStart(const FInputActionValue& Value);
	void InteractionStop(const FInputActionValue& Value);

public:
	/**
	 * @brief 긴 인터렉션이 시작되었을 때 호출됩니다. 인터렉션 시작 시간, 대상 액터를 매개변수로 받습니다.
	 * 이 이벤트는 오래 걸리는 상호작용시를 상정하여 선언되었으므로 즉시 실행되는 상호작용의 경우 호출되지 않을 수 있습니다.
	 */
	FInteractionStartSignature OnInteractionStarted;

	/**
	 * @brief 긴 인터렉션이 중단되었을 때 호출됩니다. 매개변수로 중단된 시간을 받습니다.
	 */
	FInteractionEndedSignature OnInteractionStoppedNotify;

private:
	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	class UInputMappingContext* InteractionContext;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Context")
	int8 InteractionPriority;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	class UInputAction* InteractionStartAction;

	UPROPERTY(EditAnywhere, Category = "Input|Interaction|Actions")
	UInputAction* InteractionStopAction;

	UPROPERTY(EditAnywhere, Category = Interaction)
	float InteractionRange;

	UPROPERTY(EditAnywhere, Category = Interaction)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(Config)
	float LockstepDelay = 0.1f;

	uint8 InteractableCount;
	TWeakObjectPtr<AActor> InteractingActor;
	FCollisionQueryParams TraceQueryParams;
	FTimerHandle InteractionTimer;
	FTimerHandle OwnerInteractionTimer;
	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSubSystem;
};
