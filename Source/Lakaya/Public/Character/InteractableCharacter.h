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
	TWeakObjectPtr<AActor> InteractingActor;

	// 현재 플레이어의 상호작용 상태를 나타냅니다.
	EInteractionState InteractionState = EInteractionState::None;
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
	bool ShouldInteract() const;
	
public:
	// 상호작용을 시작합니다.
	void StartInteraction();

	// 상호작용을 멈춥니다.
	void StopInteraction();

	// 상호작용이 끝남을 알려줍니다.
	void FinishInteraction(EInteractionState NewState, float Time);
	
	FORCEINLINE const EInteractionState& GetInteractionState() const { return InteractionInfo.InteractionState; }

	FORCEINLINE void SetInteractionState(const EInteractionState& NewState) { InteractionInfo.InteractionState = NewState; }	

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStart(const float& Time, AActor* Actor);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestInteractionStop(const float& Time, AActor* Actor);
	
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