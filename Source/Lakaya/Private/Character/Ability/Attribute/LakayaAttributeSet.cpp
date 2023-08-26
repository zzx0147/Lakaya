// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Attribute/LakayaAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

float ULakayaAttributeSet::GetHealth() const
{
	return FMath::Max(Health.GetCurrentValue(),0.0f);
}

void ULakayaAttributeSet::SetHealth(float NewVal)
{
	// 0 미만의 값을 받지 않습니다.
	NewVal = FMath::Max(NewVal, 0.0f);

	// 어빌리티 시스템 컴포넌트 인스턴스가 있는지 확인합니다. 항상 인스턴스가 있어야 합니다.
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetHealthAttribute(), NewVal);
	}
}

void ULakayaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 이 게임플레이 이펙트는 Health를 변경합니다. 적용하되 우선 값을 제한합니다.
		// 이 경우 Health 베이스 값은 음수가 아니어야 합니다.
		SetHealth(FMath::Max(GetHealth(), 0.0f));
	}
}

void ULakayaAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, Health, OldHealth);
}

void ULakayaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, Health, COND_None, REPNOTIFY_Always);
}
