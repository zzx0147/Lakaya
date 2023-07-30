// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "LakayaAbilitySet.h"
#include "RegisterAbilityInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Occupation/PlayerTeam.h"
#include "LakayaBaseCharacter.generated.h"

struct FGameplayAbilitySpec;

USTRUCT()
struct FPlayerRotationPacket
{
	GENERATED_BODY()

	UPROPERTY()
	FQuat Rotation;

	UPROPERTY()
	float Time;
};

UCLASS()
class LAKAYA_API ALakayaBaseCharacter : public ACharacter, public IAbilitySystemInterface,
                                        public IRegisterAbilityInterface
{
	GENERATED_BODY()

public:
	const static FName SpringArmComponentName;
	const static FName CameraComponentName;
	const static FName ResourceComponentName;
	const static FName ClairvoyanceMeshComponentName;
	const static FName DamageImmuneMeshComponentName;
	const static FName ResurrectionNiagaraName;

	explicit ALakayaBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GiveAbilities(UAbilitySystemComponent* InAbilitySystem) override;
	virtual void ClearAbilities() override;

protected:
	virtual void BeginPlay() override;
	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
	                                       AController* EventInstigator, AActor* DamageCauser) override;

public:
	// 캐릭터에 부착된 스프링암 컴포넌트를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	class USpringArmComponent* const& GetSpringArm() const { return SpringArm; }

	// 캐릭터에 부착된 카메라 컴포넌트를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	class UCameraComponent* const& GetCamera() const { return Camera; }

	// 캐릭터의 자원 컴포넌트를 가져옵니다.
	template <class T = class UResourceComponent>
	UFUNCTION(BlueprintGetter)
	T* GetResourceComponent() const { return Cast<T>(ResourceComponent); }

	// 캐릭터 고유의 최대 체력을 가져옵니다. 플레이어의 최종적인 체력을 의미하지는 않습니다.
	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxHealth() const { return MaxHealth; }

	// 연속처치시 적용될 버프 목록을 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const TArray<FName>& GetKillStreakBuffs() const { return KillStreakBuffs; }

	UFUNCTION(BlueprintGetter)
	const bool& GetAliveState() const { return bIsAlive; }

	// 현재 플레이어가 바라보는 방향 정보를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	FRotator GetPlayerRotation() const;

	UFUNCTION(BlueprintGetter)
	const FName& GetCharacterName() const { return CharacterName; }

	// 캐릭터에게 팀을 설정해줍니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetTeam(const EPlayerTeam& Team);

	// 캐릭터의 생존 상태를 변경합니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetAliveState(bool IsAlive);

	bool IsSameTeam(const EPlayerTeam& Team) const;

	void PlayHitScreen();

	// 이 캐릭터에게 강제로 투시를 활성화합니다. 가려지는 부분만 투시가 되는 것이 아니라 가려지든 말든 투시효과를 그리게 되므로 유의하여 활성화해야 합니다.
	void EnableClairvoyance();

	// 투시효과를 비활성화합니다.
	void DisableClairvoyance();

	UFUNCTION(BlueprintNativeEvent)
	void SetStencilMask(const ERendererStencilMask& StencilMask);

	void SetAlly(const bool& IsAlly);

protected:
	virtual void SetTeam_Implementation(const EPlayerTeam& Team);
	virtual void SetAliveState_Implementation(bool IsAlive);

	// 현재 시점의 서버 시간을 가져옵니다.
	float GetServerTime() const;

	UFUNCTION()
	virtual void OnRep_PlayerRotation();

	UFUNCTION()
	virtual void OnRep_DamageImmuneEndingTime();

private:
	// 단순히 이전 주기의 플레이어 회전과 최신 주기의 플레이어 회전을 선형 외삽한 값을 반환합니다.
	FQuat GetRawExtrapolatedRotator(const float& CurrentTime) const;

	void DamageImmuneTimerCallback();

	//디졸브 이펙트를 시작합니다
	void StartDissolveEffect();

	void RemoveDissolveEffect();

	UFUNCTION()
	void DissolveTick(const float& Value);

protected:
	// 이 캐릭터의 고유한 최대 체력을 나타냅니다.
	UPROPERTY(EditAnywhere)
	float MaxHealth;

	// 연속처치시 플레이어에게 적용될 버프를 지정합니다.
	UPROPERTY(EditAnywhere)
	TArray<FName> KillStreakBuffs;

	// 플레이어의 최신 회전값에 얼마나 근접하여 보간할지 지정합니다.
	// 이 값이 1에 근접할수록 반응성이 향상되지만 도약현상이 자주 보일 수 있습니다.
	// 0에 너무 가까우면 회전이 동기화되지 않는 것처럼 보일 수 있습니다.
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.1f, ClampMax = 1.0f))
	float PlayerRotationInterpolationAlpha;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* HitScreenEffect;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> ATeamObjectType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> BTeamObjectType;

	// 부활시 적용되는 무적시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float ResurrectionDamageImmuneTime;

	// 로컬 캐릭터에 대한 아웃라인을 활성화할지 여부를 선택합니다.
	UPROPERTY(EditAnywhere)
	bool bEnableLocalOutline;

	//머티리얼 디졸브 이펙트를 위한 커브입니다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	TObjectPtr<UCurveFloat> DissolveCurve;

	//디졸브 이펙트의 타겟 머티리얼들입니다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DissolveTargetArray;

	//디졸브 이펙트를 위한 타임라인입니다
	FTimeline DissolveTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	float DissolveTimelineLength;

	//캐릭터의 이름입니다
	FName CharacterName;

	/** 이 캐릭터가 사용할 어빌리티들을 지정하는 데이터 에셋입니다. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaAbilitySet> CharacterAbilities;

private:
	UPROPERTY(VisibleAnywhere, Replicated)
	class UResourceComponent* ResourceComponent;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ClairvoyanceMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DamageImmuneMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ResurrectionNiagara;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerRotation, Transient)
	FPlayerRotationPacket PlayerRotation;

	UPROPERTY(BlueprintGetter=GetAliveState)
	bool bIsAlive;

	UPROPERTY(ReplicatedUsing=OnRep_DamageImmuneEndingTime, Transient)
	float DamageImmuneEndingTime;

	FPlayerRotationPacket PrevPlayerRotation;
	FPlayerRotationPacket LatestPlayerRotation;
	FQuat LatestUpdateRotation;
	EPlayerTeam RecentTeam;
	FVector MeshRelativeLocation;
	FRotator MeshRelativeRotation;
	FName MeshCollisionProfile;
	TWeakObjectPtr<UMaterialInstanceDynamic> CharacterOverlayMaterial;
	FTimerHandle DamageImmuneTimer;
	FLakayaAbilityHandleContainer AbilityHandleContainer;
};
