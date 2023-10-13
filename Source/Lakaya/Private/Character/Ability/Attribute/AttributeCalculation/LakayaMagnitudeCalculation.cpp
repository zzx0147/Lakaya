// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Attribute/AttributeCalculation/LakayaMagnitudeCalculation.h"

float ULakayaMagnitudeCalculation::GetSetByCallerMagnitudeByTagConst(const FGameplayEffectSpec& EffectSpec,
	const FGameplayTag& Tag) const
{
	return EffectSpec.GetSetByCallerMagnitude(Tag, true, 0.0f);
}

float ULakayaMagnitudeCalculation::K2_GetCapturedAttributeMagnitudeConst(const FGameplayEffectSpec& EffectSpec,
	FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	float Magnitude = 0.0f;

	// look for the attribute in the capture list
	for (const FGameplayEffectAttributeCaptureDefinition& CurrentCapture : RelevantAttributesToCapture)
	{
		if (CurrentCapture.AttributeToCapture == Attribute)
		{
			// configure the aggregator evaluation parameters
			// TODO: apply filters?
			FAggregatorEvaluateParameters EvaluationParameters;

			EvaluationParameters.SourceTags = &SourceTags;
			EvaluationParameters.TargetTags = &TargetTags;

			// get the attribute magnitude
			GetCapturedAttributeMagnitude(CurrentCapture, EffectSpec, EvaluationParameters, Magnitude);

			// capture found. Stop the search
			break;
		}
	}

	return Magnitude;
}
