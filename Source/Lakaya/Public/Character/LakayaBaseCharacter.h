// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Occupation/PlayerTeam.h"
#include "LakayaBaseCharacter.generated.h"

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
class LAKAYA_API ALakayaBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	const static FName SpringArmComponentName;
	const static FName CameraComponentName;
	const static FName ResourceComponentName;

	explicit ALakayaBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
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

	// 현재 플레이어가 바라보는 방향 정보를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	FRotator GetPlayerRotation() const;

	// 캐릭터에게 팀을 설정해줍니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetTeam(const EPlayerTeam& Team);

	// 캐릭터의 생존 상태를 변경합니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetAliveState(bool IsAlive);

	void PlayHitScreen();

protected:
	virtual void SetTeam_Implementation(const EPlayerTeam& Team);
	virtual void SetAliveState_Implementation(bool IsAlive);

	// 현재 시점의 서버 시간을 가져옵니다.
	float GetServerTime() const;

	UFUNCTION()
	virtual void OnRep_PlayerRotation();

private:
	// 단순히 이전 주기의 플레이어 회전과 최신 주기의 플레이어 회전을 선형 외삽한 값을 반환합니다.
	FQuat GetRawExtrapolatedRotator(const float& CurrentTime) const;

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

	// 캐릭터가 부활했을 때 재생할 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ResurrectionNiagaraSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* HitScreenEffect;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> ATeamObjectType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> BTeamObjectType;

private:
	UPROPERTY(VisibleAnywhere, Replicated)
	class UResourceComponent* ResourceComponent;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerRotation, Transient)
	FPlayerRotationPacket PlayerRotation;

	FPlayerRotationPacket PrevPlayerRotation;
	FPlayerRotationPacket LatestPlayerRotation;
	FQuat LatestUpdateRotation;
};
