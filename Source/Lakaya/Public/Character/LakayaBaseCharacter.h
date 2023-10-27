// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "LakayaAbilitySet.h"
#include "LakayaBasePlayerState.h"
#include "RegisterAbilityInterface.h"
#include "Components/Image.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Occupation/Team.h"
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
	const static FName GrayScalePostProcessComponentName;
	const static FName BulletSpreadComponentName;

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

	UFUNCTION(BlueprintGetter)
	class UBulletSpreadComponent* GetBulletSpread() const { return BulletSpreadComponent; }
	
	// 캐릭터의 자원 컴포넌트를 가져옵니다.
	template <class T = class UResourceComponent>
	UFUNCTION(BlueprintGetter)
	T* GetResourceComponent() const { return Cast<T>(ResourceComponent); }

	// 캐릭터 고유의 최대 체력을 가져옵니다. 플레이어의 최종적인 체력을 의미하지는 않습니다.
	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterAttackPoint() const { return AttackPoint; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxSkillStack() const { return MaxSkillStack; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxUltimateGauge() const { return MaxUltimateGauge; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterGainUltimateOnAttack() const { return GainUltimateOnAttack; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterGainUltimateOnAttacked() const { return GainUltimateOnAttacked; }

	UFUNCTION(BlueprintGetter)
	const float& GetCharacterGainUltimateOnSecond() const { return GainUltimateOnSecond; }
	
	// 연속처치시 적용될 버프 목록을 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const TArray<FName>& GetKillStreakBuffs() const { return KillStreakBuffs; }

	// 캐릭터의 생존 여부 상태를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const bool& GetAliveState() const { return bIsAlive; }

	// 현재 플레이어가 바라보는 방향 정보를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	FRotator GetPlayerRotation() const;

	UFUNCTION(BlueprintGetter)
	const FName& GetCharacterName() const { return CharacterName; }

	// 캐릭터에게 팀을 설정해줍니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetTeam(const ETeam& Team);

	UFUNCTION(BlueprintGetter)
	ETeam GetTeam() { return RecentTeam; }
	
	// 캐릭터의 생존 상태를 변경합니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetAliveState(bool IsAlive);

	bool IsSameTeam(const ETeam& Team) const;

	void PlayHitScreen();

	// 이 캐릭터에게 강제로 투시를 활성화합니다. 가려지는 부분만 투시가 되는 것이 아니라 가려지든 말든 투시효과를 그리게 되므로 유의하여 활성화해야 합니다.
	void EnableClairvoyance();

	// 투시효과를 비활성화합니다.
	void DisableClairvoyance();

	UFUNCTION(BlueprintNativeEvent)
	void SetStencilMask(const ERendererStencilMask& StencilMask);

	void SetAlly(const bool& IsAlly);

	FORCEINLINE TSubclassOf<class UCharacterWidget> GetCharacterWidgetClass() const { return CharacterWidgetClass; }

	FORCEINLINE TSubclassOf<UGameplayEffect> GetKillStreakBuffEffect() const { return KillStreakBuffEffect; }
	
	/**
	* @brief 해당 적이 카메라에 보이는지 확인합니다.
	* @param EnemyTeam 적의 소속팀 입니다.
	* @param EnemyState 확인할 적의 상태입니다.
	* @return 카메라에 보인다면 true, 아니라면 false를 반환합니다.
	*/
	bool IsEnemyVisibleInCamera(const ETeam& EnemyTeam, const TWeakObjectPtr<ALakayaBasePlayerState> EnemyState,
		const TWeakObjectPtr<UImage> EnemyImage);

	UFUNCTION(Server, Reliable)
	void Server_OnEnemySpotted(const ETeam& EnemyTeam, ALakayaBasePlayerState* EnemyState);

	UFUNCTION(Server, Reliable)
	void Server_OnEnemyLost(const ETeam& EnemyTeam, ALakayaBasePlayerState* EnemyState);

	// VisibleEnemy가 비어있는지 확인합니다.
	FORCEINLINE const bool IsVisibleEnemyEmpty() const { return VisibleEnemies.Num() == 0; }
	
	// VisibleEnemy 목록을 반환합니다.
	FORCEINLINE const TSet<ALakayaBasePlayerState*>& GetVisibleEnemies() const { return VisibleEnemies; }
	
	// 캐릭터 시야에 적이 들어왔다면 VisibleEnemies에 추가합니다.
	void AddVisibleEnemy(ALakayaBasePlayerState* Enemy) { VisibleEnemies.Emplace(Enemy); }

	// 캐릭터 시야에서 적이 나갔다면 VisibleEnemies에서 제거합니다.
	void RemoveVisibleEnemy(ALakayaBasePlayerState* Enemy) { VisibleEnemies.Remove(Enemy); }

	// 캐릭터 시야에 적이 있는지 확인합니다.
	bool IsEnemyVisible(ALakayaBasePlayerState* Enemy) const { return VisibleEnemies.Contains(Enemy); }

	UFUNCTION(Server, Reliable)
	void Server_SetEnemyVisibility(ALakayaBasePlayerState* EnemyState, bool bIsVisible);

	UFUNCTION(Client, Reliable)
	void Client_SetEnemyVisibility(ALakayaBasePlayerState* EnemyState, bool bIsVisible);
protected:
	virtual void SetTeam_Implementation(const ETeam& Team);
	virtual void SetAliveState_Implementation(bool IsAlive);

	// 현재 시점의 서버 시간을 가져옵니다.
	float GetServerTime() const;

	UFUNCTION()
	virtual void OnRep_PlayerRotation();

	UFUNCTION()
	virtual void OnRep_DamageImmuneEndingTime();

	UFUNCTION()
	virtual bool CanJumpInternal_Implementation() const override;

private:
	// 단순히 이전 주기의 플레이어 회전과 최신 주기의 플레이어 회전을 선형 외삽한 값을 반환합니다.
	FQuat GetRawExtrapolatedRotator(const float& CurrentTime) const;

	void DamageImmuneTimerCallback();

	//디졸브 이펙트를 시작합니다
	void StartDissolveEffect();

	void RemoveDissolveEffect();

	void ToggleGrayScalePostProcess(const bool& bIsActivate);
	
	UFUNCTION()
	void DissolveTick(const float& Value);

protected:
	// 이 캐릭터의 고유한 최대 체력을 나타냅니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float MaxHealth;

	//TODO: 캐릭터가 가진 기본 스탯을 어빌리티 셋에 설정하도록 로직을 추가하여야 합니다
	// 이 캐릭터의 최대 총알 갯수의 기본값입니다. AttributeSet의 기본 값을 설정하는데 사용됩니다. 런타임중에 변경하지 마십시오
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float MaxAmmo;

	//스택형 스킬의 리소스입니다. 각각 레나는 지뢰, 와지는 연막탄, 강림은 대쉬의 스택입니다
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float MaxSkillStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float MaxUltimateGauge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float GainUltimateOnAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float GainUltimateOnAttacked;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float GainUltimateOnSecond;
	
	// 이 캐릭터의 공격력의 기본값입니다. AttributeSet의 기본 값을 설정하는데 사용됩니다. 런타임중에 변경하지 마십시오
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LakayaCharacterStat, meta=(AllowPrivateAccess = true))
	float AttackPoint;
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCharacterWidget> CharacterWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> KillStreakBuffEffect;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> GrayScalePostProcessMaterial;
	
private:
	UPROPERTY(VisibleAnywhere, Replicated)
	class UResourceComponent* ResourceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetSpringArm, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetCamera, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ClairvoyanceMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DamageImmuneMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ResurrectionNiagara;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPostProcessComponent> GrayScalePostProcessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetBulletSpread)
	TObjectPtr<UBulletSpreadComponent> BulletSpreadComponent;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerRotation, Transient)
	FPlayerRotationPacket PlayerRotation;

	UPROPERTY(BlueprintGetter=GetAliveState)
	bool bIsAlive;

	UPROPERTY(ReplicatedUsing=OnRep_DamageImmuneEndingTime, Transient)
	float DamageImmuneEndingTime;

	FPlayerRotationPacket PrevPlayerRotation;
	FPlayerRotationPacket LatestPlayerRotation;
	FQuat LatestUpdateRotation;
	ETeam RecentTeam;
	FVector MeshRelativeLocation;
	FRotator MeshRelativeRotation;
	FName MeshCollisionProfile;
	TWeakObjectPtr<UMaterialInstanceDynamic> CharacterOverlayMaterial;
	FTimerHandle DamageImmuneTimer;
	FLakayaAbilityHandleContainer AbilityHandleContainer;

	// 플레이어가 적을 발견했을 때, 시야를 공유하기 위해 사용하는 변수입니다.
	UPROPERTY()
	TSet<ALakayaBasePlayerState*> VisibleEnemies;

	UPROPERTY()
	TObjectPtr<UTexture2D> QuestionIcon;
};
