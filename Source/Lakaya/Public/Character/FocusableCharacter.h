// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "FocusableCharacter.generated.h"

UENUM(BlueprintType)
enum class EFocusContext : uint8
{
	//서버측에서 시뮬레이트되고있는 FocusSpace를 구분하기 위해 사용됩니다.
	Server,

	//클라이언트에서, 내 소유가 아닌 캐릭터의 FocusSpace를 구분하기 위해 사용됩니다.
	Simulated,

	//내 소유의 클라이언트에서, 미래시점의 FocusSpace를 시뮬레이트 하기 위해 사용됩니다.
	Owner
};

UENUM(BlueprintType)
enum class EFocusSpace : uint8
{
	//손으로 하는 주된 행동을 제약하기 위해 사용됩니다. 예를 들어, 재장전, 사격, 조정간 조정 등이 동시에 이뤄질 수 없게 할 때 사용됩니다.
	MainHand
};

UENUM(BlueprintType)
enum class EFocusState : uint8
{
	//FocusSpace가 비어있음을 의미합니다. 다른 행동을 수행할 수 있습니다.
	None = 0,

	//사격중임을 나타냅니다.
	Firing,

	//조정간 조정중임을 나타냅니다.
	Switching,

	//재장전중임을 나타냅니다.
	Reloading,

	//상호작용중임을 나타냅니다.
	Interacting
};

UCLASS()
class LAKAYA_API AFocusableCharacter : public AMovableCharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief FocusSpace에 점유를 시도합니다.
	 * @param Context 점유를 시도할 컨텍스트입니다.
	 * @param Space 점유를 시도할 FocusSpace입니다.
	 * @param State 어느 FocusState로 지정할지 선택합니다.
	 * @return 점유에 성공하면 true, 실패하면 false를 반환합니다. 같은 State에 의해 점유된 상태이더라도 false를 반환합니다.
	 */
	bool SetFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State);

	/**
	 * @brief 강제로 FocusSpace를 점유합니다. 기존 State를 무시하긴 하지만, 로그를 남깁니다.
	 * @param Context 점유할 컨텍스트입니다.
	 * @param Space 점유할 FocusSpace입니다.
	 * @param State 어느 FocusState로 점유할지 선택합니다.
	 */
	void SetFocusForce(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State);

	/**
	 * @brief FocusSpace에서 점유 해제를 시도합니다.
	 * @param Context 점유 해제를 시도할 컨텍스트입니다.
	 * @param Space 점유를 해제할 FocusSpace입니다.
	 * @param State 점유시 사용한 FocusState와 다른 경우 점유 해제에 실패합니다.
	 * @return 점유 해제 성공여부를 나타냅니다. 만약 FocusState가 None이었던 경우 false가 반환됩니다.
	 */
	bool ReleaseFocus(const EFocusContext& Context, const EFocusSpace& Space, const EFocusState& State);


	/**
	 * @brief FocusSpace에 대해 강제로 점유를 해제합니다.
	 * @param Context 점유를 해제할 컨텍스트입니다.
	 * @param Space 점유를 해제할 FocusSpace입니다. 
	 * @param State 디버깅 옵션입니다. FocusState와 다른 경우 로그를 남깁니다.
	 */
	void ReleaseFocusForce(const EFocusContext& Context, const EFocusSpace& Space,
	                       const EFocusState& State = EFocusState::None);

	/**
	 * @brief 현재 FocusSpace가 사용되고 있는지 여부를 조사합니다.
	 * @param Context 조사할 컨텍스트입니다.
	 * @param Space 조사할 FocusSpace입니다.
	 * @param State 예외로 설정할 FocusState입니다.
	 * @return FocusSpace가 None이거나 State와 같다면 false, 이외의 다른 상태인 경우 true를 반환합니다.
	 */
	bool IsFocussed(const EFocusContext& Context, const EFocusSpace& Space,
	                const EFocusState& State = EFocusState::None) const;

	/**
	 * @brief FocusSpace가 특정 FocusState에 의해 점유되었는지 조사합니다.
	 * 기본값으로 State가 None으로 되어 있는데, None인 경우 이 함수는 FocusSpace가 비어있는지 조사하는 함수로 동작합니다.
	 * @param Context 조사할 컨텍스트입니다.
	 * @param Space 조사할 FocusSpace입니다.
	 * @param State 이 FocusState에 의해 점유되었는지 조사합니다.
	 * @return FocusState가 State와 같다면 true, 그렇지 않다면 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsFocussedBy(const EFocusContext& Context, const EFocusSpace& Space,
	                  const EFocusState& State = EFocusState::None) const;

protected:
	/**
	 * @brief 어떤 FocusSpace의 상태를 받아옵니다.
	 * @param Context FocusSpace의 컨텍스트입니다.
	 * @param Space 상태를 받아올 FocusSpace입니다.
	 * @return 현재 FocusState입니다.
	 */
	inline EFocusState& GetFocusState(const EFocusContext& Context, const EFocusSpace& Space)
	{
		return FocusMap.FindOrAdd(Context).FindOrAdd(Space);
	}

private:
	TMap<EFocusContext, TMap<EFocusSpace, EFocusState>> FocusMap;
};
