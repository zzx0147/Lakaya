#pragma once
#include "CoreMinimal.h"
#include "Character/InteractableCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class AInteractable : public AActor
{
	GENERATED_BODY()

public:
	explicit AInteractable(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * @brief 이 액터와 인터렉션을 시작합니다. 이 함수는 서버에서 호출됩니다.
	 * @param Time 클라이언트에서 이 액터와 인터렉션을 시작한 시간입니다.
	 * @param Caller 인터렉션을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionStart(const float& Time, APawn* Caller) { return; }

	/**
	 * @brief 이 액터와 인터렉션을 중단합니다. 이 함수는 서버에서 호출됩니다.
	 * @param Time 클라이언트에서 이 액터와 인터렉션을 중단한 시간입니다.
	 * @param Caller 인터렉션 중단을 요청한 캐릭터입니다.
	 * @param NewState 중단됐을 때, 상호작용 성공여부입니다. 
	 */
	virtual void OnInteractionStop(const float& Time, APawn* Caller, EInteractionState NewState) { return; }
	
	/**
	 * @brief 인터렉션 중이던 캐릭터가 사망하면 호출됩니다. 이 함수는 서버측에서 호출됩니다.
	 * @param Caller 사망한 캐릭터입니다.
	 */
	virtual void OnCharacterDead(APawn* Caller) { return; }

	/**
	 * @brief 상호작용이 성공 했을 때, 호출 됩니다.
	 * @param Caller 상호작용을 성공한 캐릭터입니다.
	 */
	virtual void OnInteractionFinish(APawn* Caller) { return; }


protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	USphereComponent* Trigger;
	
	APawn* InteractingPawn;
	TWeakObjectPtr<APawn> OwnerPlayer;
};