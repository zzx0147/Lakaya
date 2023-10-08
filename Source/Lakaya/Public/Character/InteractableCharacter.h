#pragma once

#include "CoreMinimal.h"
#include "ArmedCharacter.h"
#include "InteractableCharacter.generated.h"

DECLARE_EVENT_OneParam(AInteractableCharacter, FInteractionSignature, AActor*);
DECLARE_EVENT_OneParam(AInteractableCharacter, FOnInteractionStateChanged, FInteractionInfo);

UENUM()
enum class EInteractionState : uint8
{
	None UMETA(DisplayerName = "None"),
	OnGoing UMETA(DisplayerName = "OnGoing"),
	Success UMETA(DisPlayerName = "Success"),
	Stopped UMETA(DisPlayerName = "Stopped"),
	Canceled UMETA(DisPlayerName = "Canceled"),
};

USTRUCT()
struct FInteractionInfo
{
	GENERATED_BODY();

	// 현재 인터랙션 중인 액터입니다.
	UPROPERTY()
	TWeakObjectPtr<AActor> InteractingActor;

	// 현재 플레이어의 상호작용 상태를 나타냅니다.
	UPROPERTY()
	EInteractionState InteractionState = EInteractionState::None;

	// 현재 인터렉션 상태가 유지되는 목표 시간입니다.
	// UPROPERTY()
	// float EndingTime;
};

UCLASS(Config = Game)
class LAKAYA_API AInteractableCharacter : public AArmedCharacter
{
	GENERATED_BODY()

public:
	explicit AInteractableCharacter(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	// 현재 상호작용을 할 수 있는지 판별합니다.
	virtual bool ShouldInteract() const;
	
public:
	/**
	 * @brief 상호작용을 요청합니다.
	 */
	void StartInteraction();

	/**
	 * @brief 상호작용 중단을 요청합니다.
	 * @param NewState 상호작용이 중단되었을 때, 상호작용 성공 여부를 나타냅니다. 
	 */
	void StopInteraction(EInteractionState NewState);

	// 상호작용이 끝남을 알려줍니다.
	// 상호작용이 끝나게 되면 실행되는 함수입니다.
	/**
	 * @brief 상호작용이 끝남을 알려줍니다.
	 * @param NewState 상호작용이 끝났을 시, 성공 여부입니다.
	 * @param Time 상호작용이 끝난 시간입니다.
	 * 
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void FinishInteraction(EInteractionState NewState, float Time);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetInteractingActor() const { return InteractionInfo.InteractingActor.Get(); }
	
	FORCEINLINE const EInteractionState& GetInteractionState() const { return InteractionInfo.InteractionState; }

	FORCEINLINE void SetInteractionState(const EInteractionState& NewState) { InteractionInfo.InteractionState = NewState; }

	FORCEINLINE bool IsNotInteracting() const { return InteractionInfo.InteractionState == EInteractionState::None; }

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	/**
	 * @brief 클라에게 중단 요청을 받게 되면 서버에서 실행됩니다.
	 * @param Time 중단 요청을 받은 시간입니다.
	 * @param Actor 중단을 요청한 캐릭터입니다.
	 * @param NewState 중단을 했을 때, 상호작용 성공 여부를 나타냅니다.
	 * 
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStop(const float& Time, AActor* Actor, EInteractionState NewState);

private:
	// 인터렉션이 가능한 액터입니다.
	TWeakObjectPtr<AActor> InteractableActor;

	UPROPERTY(ReplicatedUsing = OnRep_InteractingActor)
	FInteractionInfo InteractionInfo;
	
	// 현재 본인의 상호작용 상태를 나타냅니다.
	UPROPERTY()
	bool bInteractionRequested;
	
	UFUNCTION()
	void OnRep_InteractingActor() const;

public:
	// 인터렉션이 가능한 액터가 변경되는 경우 호출됩니다. 매개변수로 넘겨진 액터가 nullptr이면 인터렉션이 불가능해졌음을 의미하며,
	// 그렇지 않은 경우 해당 액터와 인터렉션이 가능해졌음을 의미합니다.
	FInteractionSignature OnInteractableActorChanged;

	// 인터렉션중인 액터가 변화한 경우 호출됩니다. 매개변수로 넘겨진 액터가 nullptr이면 인터렉션 중단을 의미하며,
	// 그렇지 않은 경우 인터렉션중임을 의미합니다.
	FInteractionSignature OnInteractingActorChanged;

	// 상호작용 상태가 변화한 경우 호출됩니다. 매개변수로 넘겨진 액터가 nullptr이면 인터렉션 중단을 의미합니다.
	FOnInteractionStateChanged OnInteractionStateChanged;
	
	FTimerHandle InteractionClearTimer;
};