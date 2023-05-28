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
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreCount;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 KillCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 DeathCount;

	bool operator<(const FPlayerData& Other) const
	{
		if (ScoreCount != Other.ScoreCount)
			return ScoreCount < Other.ScoreCount;
		else if (KillCount != Other.KillCount)
			return KillCount < Other.KillCount;
		else if (DeathCount != Other.DeathCount)
			return DeathCount < Other.DeathCount;
	}
};

UCLASS()
class LAKAYA_API UIndividualBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	
protected:
	
	
private:
	
	
	
};
