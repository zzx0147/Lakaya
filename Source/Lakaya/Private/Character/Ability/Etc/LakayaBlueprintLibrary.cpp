// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Etc/LakayaBlueprintLibrary.h"

#include "GameplayEffectTypes.h"
#include "Input/LakayaInputID.h"

int32 ULakayaBlueprintLibrary::GetInputID(const FLakayaInputID& InputID)
{
	return InputID.GetInputID();
}

void ULakayaBlueprintLibrary::SetSourceObject(FGameplayEffectContextHandle EffectContext, const UObject* SourceObject)
{
	EffectContext.AddSourceObject(SourceObject);
}

ECollisionChannel ULakayaBlueprintLibrary::ConvertToCollisionChannel(const EObjectTypeQuery& ObjectType)
{
	return UEngineTypes::ConvertToCollisionChannel(ObjectType);
}

FGameplayEffectContextHandle ULakayaBlueprintLibrary::GetEffectContext(const FGameplayEffectSpec& EffectSpec)
{
	return EffectSpec.GetEffectContext();
}

void ULakayaBlueprintLibrary::GetChildActors(const AActor* ParentActor, TArray<AActor*>& OutChildren)
{
	if (IsValid(ParentActor))
	{
		Algo::Copy(ParentActor->Children, OutChildren);
	}
}
