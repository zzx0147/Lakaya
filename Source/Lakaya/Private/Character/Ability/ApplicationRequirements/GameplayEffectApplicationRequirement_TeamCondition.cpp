// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ApplicationRequirements/GameplayEffectApplicationRequirement_TeamCondition.h"

#include "AbilitySystemComponent.h"
#include "Interface/TeamObjectInterface.h"

bool UGameplayEffectApplicationRequirement_TeamCondition::CanApplyGameplayEffect_Implementation(
	const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const
{
	// 팀 오브젝트 인터페이스를 구현할만한 UObject 목록을 통해 팀 오브젝트를 찾습니다.
	static const auto FindTeamObject = [](const TArray<const UObject*>& Objects)
	{
		static const auto Pred = [](const UObject* Object)
		{
			return IsValid(Object) && Object->Implements<UTeamObjectInterface>();
		};
		return Cast<ITeamObjectInterface>(Objects.FindByPredicate(Pred));
	};

	const auto ASCTeamObject = FindTeamObject({ASC->GetOwnerActor(), ASC->GetAvatarActor()});
	if (!ASCTeamObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find TeamObjectInterface from ASC. GameplayEffect will be ignored"));
		return false;
	}

	const auto& Context = Spec.GetEffectContext();
	const auto SpecTeamObject = FindTeamObject({Context.GetInstigator(), Context.GetEffectCauser()});
	if (!SpecTeamObject)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Failed to find TeamObjectInterface from EffectContext. GameplayEffect will be ignored"));
		return false;
	}

	// 다른 팀인 경우에만 이펙트를 적용할 수 있도록 합니다.
	return !ASCTeamObject->IsSameTeam(SpecTeamObject);
}
