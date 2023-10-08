// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Attribute/LakayaBaseAttributeSet.h"

#include "AbilitySystemComponent.h"

void ULakayaBaseAttributeSet::SetupRelatedAttributes()
{
	for (auto&& RelatedData : RelatedAttributes)
	{
		MapByBase.Add(RelatedData.BaseAttribute, &RelatedData);
		MapByMax.Add(RelatedData.MaxAttribute, &RelatedData);
	}
}

void ULakayaBaseAttributeSet::UpdateMaxTagOnReplicated(const FGameplayAttribute& Attribute)
{
	auto RelatedData = MapByBase.Find(Attribute);
	if (!RelatedData)
	{
		RelatedData = MapByMax.Find(Attribute);
		if (!RelatedData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot find related attribute data on UpdateMaxTagOnReplicated!"));
			return;
		}
	}

	const auto StoredData = *RelatedData;
	if (const auto& Tag = StoredData->MaxReachedTag; Tag && Tag->IsValid())
	{
		UpdateMaxTag(*Tag, StoredData->BaseAttribute.GetNumericValueChecked(this),
		             StoredData->MaxAttribute.GetNumericValueChecked(this), StoredData->NearRange);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found invalid tag on UpdateMaxTagOnReplicated!"));
	}
}

void ULakayaBaseAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void ULakayaBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void ULakayaBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 베이스 애트리뷰트가 변경된 경우 단순히 태그 업데이트만 수행합니다.
	if (auto RelatedData = MapByBase.Find(Attribute))
	{
		const auto StoredData = *RelatedData;
		if (const auto& Tag = StoredData->MaxReachedTag; Tag && Tag->IsValid())
		{
			UpdateMaxTag(*Tag, NewValue, StoredData->MaxAttribute.GetNumericValueChecked(this), StoredData->NearRange);
		}
	}
	// 최대 애트리뷰트가 변경된 경우 베이스 애트리뷰트를 업데이트하거나 태그 업데이트를 수행합니다.
	else if (RelatedData = MapByMax.Find(Attribute); RelatedData)
	{
		const auto StoredData = *RelatedData;
		const auto BaseValue = StoredData->BaseAttribute.GetNumericValueChecked(this);
		if (BaseValue > NewValue)
		{
			// 이 과정에서 MapByBase에서 애트리뷰트를 찾아 UpdateMaxTag를 호출할 것이므로 여기서는 하지 않습니다.
			GetOwningAbilitySystemComponentChecked()->SetNumericAttributeBase(StoredData->BaseAttribute, NewValue);
		}
		else if (const auto& Tag = StoredData->MaxReachedTag; Tag && Tag->IsValid())
		{
			UpdateMaxTag(*Tag, BaseValue, NewValue, StoredData->NearRange);
		}
	}
}

void ULakayaBaseAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (const auto RelatedData = MapByBase.Find(Attribute))
	{
		const auto StoredData = *RelatedData;
		NewValue = FMath::Clamp(NewValue, 0.f, StoredData->MaxAttribute.GetNumericValueChecked(this));
	}
}

void ULakayaBaseAttributeSet::UpdateMaxTag(const FGameplayTag& Tag, const float& Base, const float& Max,
                                           const float& NearRange) const
{
	const auto MaxReached = Base > Max || FMath::IsNearlyEqual(Base, Max, NearRange);
	GetOwningAbilitySystemComponentChecked()->SetLooseGameplayTagCount(Tag, MaxReached ? 1 : 0);
}
