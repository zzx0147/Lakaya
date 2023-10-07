// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "LakayaBaseAttributeSet.generated.h"

struct FRelatedAttributeData
{
	/** 영향을 받을 대상 애트리뷰트입니다. */
	FGameplayAttribute BaseAttribute;

	/** 이 애트리뷰트를 통해 대상 애트리뷰트의 최대 값을 제한합니다. */
	FGameplayAttribute MaxAttribute;

	/** 대상 애트리뷰트가 최대 값에 도달하면 이 태그를 부여합니다. 대상 애트리뷰트가 상한에서 멀어지면 자동으로 제거됩니다. */
	const FGameplayTag* MaxReachedTag;

	/** 대상 애트리뷰트와 최대 값에 대해 IsNearlyEqual을 수행할 때 허용할 범위를 지정합니다. */
	float NearRange = UE_SMALL_NUMBER;
};

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	/** 생성자에서 RelatedAttributes에 필요한 정보들을 기입한 뒤에 반드시 호출되어야 합니다. */
	void SetupRelatedAttributes();

	/** OnRep에서 이 함수를 호출해서 클라이언트에서도 MaxReachedTag가 추가되도록 할 수 있습니다. */
	void UpdateMaxTagOnReplicated(const FGameplayAttribute& Attribute);

	/** 하위 클래스의 생성자에서 이 배열에 적절한 정보를 기입하고 SetupRelatedAttributes를 호출하면 됩니다. */
	TArray<FRelatedAttributeData> RelatedAttributes;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	void UpdateMaxTag(const FGameplayTag& Tag, const float& Base, const float& Max, const float& NearRange) const;

	TMap<FGameplayAttribute, FRelatedAttributeData*> MapByBase;
	TMap<FGameplayAttribute, FRelatedAttributeData*> MapByMax;
};
