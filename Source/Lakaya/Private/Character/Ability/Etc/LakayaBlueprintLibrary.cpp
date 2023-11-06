// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Etc/LakayaBlueprintLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Input/LakayaInputID.h"
#include "Interfaces/OnlineSessionInterface.h"

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

UObject* ULakayaBlueprintLibrary::GetClassDefaultObject(TSubclassOf<UObject> Class)
{
	return Class->GetDefaultObject();
}

void ULakayaBlueprintLibrary::K2_ClientTravel(APlayerController* Controller, FString ConnectString)
{
	if (Controller)
	{
		Controller->ClientTravel(ConnectString, TRAVEL_Absolute);
	}
}

int32 ULakayaBlueprintLibrary::GetCurrentSessionPlayerCount()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		return 0;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return 0;
	}

	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (!CurrentSession)
	{
		return 0;
	}

	FOnlineSessionSettings SessionSettings = CurrentSession->SessionSettings;

	int32 NumPlayers = SessionSettings.NumPublicConnections - CurrentSession->NumOpenPublicConnections;

	return NumPlayers;
}

FGameplayAbilityActorInfo ULakayaBlueprintLibrary::GetActorInfoFromAbilitySystemComponent(
	UAbilitySystemComponent* AbilitySystemComponent)
{
	return IsValid(AbilitySystemComponent) ? *AbilitySystemComponent->AbilityActorInfo : FGameplayAbilityActorInfo();
}

bool ULakayaBlueprintLibrary::GetAttributeModifierMagnitude(const FGameplayEffectSpec& Spec, FGameplayAttribute Attribute,
                                                          float& OutMagnitude)
{
	if (IsValid(Spec.Def))
	{
		const auto Pred = [&Attribute](const auto& Modifier) { return Modifier.Attribute == Attribute; };

		if (const auto Index = Spec.Def->Modifiers.FindLastByPredicate(Pred); Index != INDEX_NONE)
		{
			return Spec.Def->Modifiers[Index].ModifierMagnitude.AttemptCalculateMagnitude(Spec, OutMagnitude);
		}
	}
	return false;
}
