// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterSetupData.h"
#include "GameFramework/Character.h"
#include "LakayaBaseCharacter.generated.h"

DECLARE_EVENT_OneParam(ALakayaBaseCharacter, FCharacterNameSignature, const FName&)

DECLARE_EVENT_OneParam(ALakayaBaseCharacter, FResourceSignature, const class UResourceComponent* const&)

DECLARE_EVENT_OneParam(ALakayaBaseCharacter, FStatSignature, const class UStatComponent* const&)


UCLASS()
class LAKAYA_API ALakayaBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALakayaBaseCharacter();

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

	// 캐릭터에 부착된 스프링암 컴포넌트를 가져옵니다.
	class USpringArmComponent* const& GetSpringArm() const { return SpringArm; }

	// 캐릭터에 부착된 카메라 컴포넌트를 가져옵니다.
	class UCameraComponent* const& GetCamera() const { return Camera; }

	// 캐릭터의 이름을 가져옵니다. 이 이름은 다른 데이터 테이블에서 기본 키로 사용될 수 있습니다.
	const FName& GetCharacterName() const { return CharacterName; }

	// 캐릭터의 자원 컴포넌트를 가져옵니다. 
	UResourceComponent* const& GetResourceComponent() const { return ResourceComponent; }

	// 캐릭터의 자원 컴포넌트를 캐스팅하여 가져옵니다.
	template <class T>
	T* GetResourceComponent() const { return Cast<T>(ResourceComponent); }

	// 캐릭터의 스탯 컴포넌트를 가져옵니다.
	UStatComponent* const& GetStatComponent() const { return StatComponent; }

	// 캐릭터를 캐릭터의 이름으로 셋업합니다. 이 함수는 서버에서 호출되어야 합니다.
	void SetupCharacter(const FName& RowName);

protected:
	UFUNCTION()
	virtual void OnRep_CharacterName();

	UFUNCTION()
	virtual void OnRep_ResourceComponent();

	UFUNCTION()
	virtual void OnRep_StatComponent();

	/**
	 * @brief 리플리케이트 되는 액터 컴포넌트를 생성하여 반환합니다.
	 * @tparam T 생성할 액터 컴포넌트의 형식이자, 반환받을 포인터의 형식입니다. 반드시 UActorComponent의 파생 클래스여야 합니다.
	 * @param ComponentClass 생성할 액터 컴포넌트의 UClass형식입니다.
	 * @return 생성된 리플리케이트되는 액터 컴포넌트입니다. 매개변수나 타입이 잘못지정된 경우 nullptr을 반환할 수 있습니다.
	 */
	template <class T>
	T* CreateReplicatedComponent(const TSubclassOf<T>& ComponentClass);

	/**
	 * @brief 서버측에서 호출되는 캐릭터 셋업 함수입니다.
	 * @param Data 캐릭터 셋업을 위한 데이터입니다.
	 */
	virtual void SetupCharacterServer(const FCharacterSetupData* Data);

	/**
	 * @brief 클라이언트 측에서 호출되는 캐릭터 셋업 함수입니다.
	 * @param Data 캐릭터 셋업을 위한 데이터입니다.
	 */
	virtual void SetupCharacterClient(const FCharacterSetupData* Data);

	/**
	 * @brief 이 캐릭터의 정보를 받아 화면에 보여질 액터를 초기화합니다.
	 * @param ActorClass 생성할 액터의 UClass입니다.
	 */
	virtual void SetupMeshActor(const TSubclassOf<AActor>& ActorClass);

public:
	// 캐릭터 이름이 변경되면 호출됩니다.
	FCharacterNameSignature OnCharacterNameChanged;

	// 자원 컴포넌트가 변경되면 호출됩니다.
	FResourceSignature OnResourceChanged;

	// 스탯 컴포넌트가 변경되면 호출됩니다.
	FStatSignature OnStatChanged;

protected:
	// 캐릭터 셋업을 진행할 데이터 테이블입니다.
	UPROPERTY(EditAnywhere)
	UDataTable* CharacterSetupTable;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_CharacterName)
	FName CharacterName;

	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing=OnRep_ResourceComponent)
	UResourceComponent* ResourceComponent;

	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing=OnRep_StatComponent)
	UStatComponent* StatComponent;
};

template <class T>
T* ALakayaBaseCharacter::CreateReplicatedComponent(const TSubclassOf<T>& ComponentClass)
{
	if (const auto Ability = Cast<T>(AddComponentByClass(ComponentClass, false, FTransform::Identity, false)))
	{
		Ability->SetIsReplicated(true);
		return Ability;
	}
	return nullptr;
}
