// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageableCharacter.h"
#include "ArmedCharacter.generated.h"

UENUM()
enum EAbilityKind : uint8
{
	// 캐릭터의 첫번째 능력입니다.
	Primary,

	// 캐릭터의 두번째 능력입니다.
	Secondary,

	// 캐릭터의 무기 기본 능력입니다.
	WeaponFire,

	// 캐릭터의 무기 보조 능력입니다.
	WeaponAbility,

	// 캐릭터의 무기 재장전 또는 스킬 능력입니다.
	WeaponReload
};

DECLARE_EVENT_OneParam(AArmedCharacter, FAbilitiesSignature, const TArray<class UCharacterAbility*>&)

DECLARE_EVENT_OneParam(AArmedCharacter, FResourceSignature, const class UResourceComponent* const&)

DECLARE_EVENT_OneParam(AArmedCharacter, FCharacterNameSignature, const FName&);

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public ADamageableCharacter
{
	GENERATED_BODY()

public:
	AArmedCharacter();

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

protected:
	virtual void BeginPlay() override;
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser) override;
	virtual void RespawnNotify_Implementation() override;

public:
	// 캐릭터를 캐릭터의 이름으로 셋업합니다. 이 함수는 서버에서 호출되어야 합니다.
	void SetupCharacter(const FName& RowName);

	/**
	 * @brief 캐릭터의 능력을 사용합니다. (키 누름)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	void StartAbility(const EAbilityKind& Kind);

	/**
	 * @brief 캐릭터의 능력 사용을 중단합니다. (키 뗌)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	void StopAbility(const EAbilityKind& Kind);

	// 캐릭터의 능력들을 가져옵니다. 이렇게 가져온 배열은 EAbilityKind를 통해 특정할 수 있습니다.
	const TArray<UCharacterAbility*>& GetAbilities() const { return Abilities; }

	// 캐릭터의 자원 컴포넌트를 가져옵니다. 
	UResourceComponent* const& GetResourceComponent() const { return ResourceComponent; }

	// 캐릭터의 이름을 가져옵니다. 이 이름은 다른 데이터 테이블에서 기본 키로 사용될 수 있습니다.
	const FName& GetCharacterName() const { return CharacterName; }

protected:
	/**
	 * @brief 리플리케이트 되는 액터 컴포넌트를 생성하여 반환합니다.
	 * @tparam T 생성할 액터 컴포넌트의 형식이자, 반환받을 포인터의 형식입니다. 반드시 UActorComponent의 파생 클래스여야 합니다.
	 * @param ComponentClass 생성할 액터 컴포넌트의 UClass형식입니다.
	 * @return 생성된 리플리케이트되는 액터 컴포넌트입니다. 매개변수나 타입이 잘못지정된 경우 nullptr을 반환할 수 있습니다.
	 */
	template <class T>
	T* CreateReplicatedComponent(const TSubclassOf<T>& ComponentClass);

	UFUNCTION()
	virtual void OnRep_Abilities();

	UFUNCTION()
	virtual void OnRep_ResourceComponent();

	UFUNCTION()
	virtual void OnRep_CharacterName();

private:
	/**
	 * @brief 이 캐릭터의 정보를 받아 화면에 보여질 액터를 초기화합니다.
	 * @param ActorClass 생성할 액터의 UClass입니다.
	 */
	void SetupMeshActor(const TSubclassOf<AActor>& ActorClass);

public:
	// 능력이 변경되면 호출됩니다. 매개변수로 전체 능력들을 넘겨주기 때문에 정확히 어떤 능력이 변경된 것인지는 알 수 없습니다.
	FAbilitiesSignature OnAbilitiesChanged;

	// 자원 컴포넌트가 변경되면 호출됩니다.
	FResourceSignature OnResourceChanged;

	// 캐릭터 이름이 변경되면 호출됩니다.
	FCharacterNameSignature OnCharacterNameChanged;

private:
	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing=OnRep_Abilities)
	TArray<UCharacterAbility*> Abilities;

	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing=OnRep_ResourceComponent)
	UResourceComponent* ResourceComponent;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_CharacterName)
	FName CharacterName;

	UPROPERTY(EditAnywhere)
	class UDataTable* CharacterSetupTable;
};

template <class T>
T* AArmedCharacter::CreateReplicatedComponent(const TSubclassOf<T>& ComponentClass)
{
	if (const auto Ability = Cast<T>(AddComponentByClass(ComponentClass, false, FTransform::Identity, false)))
	{
		Ability->SetIsReplicated(true);
		return Ability;
	}
	return nullptr;
}
