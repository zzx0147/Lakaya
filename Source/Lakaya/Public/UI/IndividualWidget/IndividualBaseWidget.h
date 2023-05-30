// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndividualBaseWidget.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Rank;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreCount;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 KillCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 DeathCount;

	bool operator<(const FPlayerData& Other) const
	{
		if (Rank != Other.Rank)
			return Rank < Other.Rank;
		else if (ScoreCount != Other.ScoreCount)
			return ScoreCount < Other.ScoreCount;
		else if (KillCount != Other.KillCount)
			return KillCount < Other.KillCount;
		else if (DeathCount != Other.DeathCount)
			return DeathCount < Other.DeathCount;
		else return false;
	}
};

USTRUCT(BlueprintType)
struct FPlayerAIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 KillCount;

	bool operator<(const FPlayerAIData& Other) const
	{
		return KillCount < Other.KillCount;
	}
};

UCLASS()
class LAKAYA_API UIndividualBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	
protected:
	
	
private:
	template <class T>
	void InitializeWidgetPtr(TWeakObjectPtr<T>& Ptr, const FName& WidgetName)
	{
		Ptr = Cast<T>(GetWidgetFromName(WidgetName));
		if (Ptr.IsStale()) UE_LOG(LogInit, Error, TEXT("Fail to find %s!"), *WidgetName.ToString());
	}
	
	
};
