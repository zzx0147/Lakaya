// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAIIndividualScoreBoardWidget.generated.h"

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
	int32 KillCount;

	bool operator<(const FPlayerData& Other) const
	{
		return KillCount < Other.KillCount;
	}
};

class UTextBlock;

UCLASS()
class LAKAYA_API UGameAIIndividualScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UGameAIIndividualScoreBoardWidget(const FObjectInitializer& ObjectInitializer);
	
	void SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score1stNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score2ndNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score3rdNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score4thNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score5thNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score6thNameTextBlock;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score1stKillCountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score2ndKillCountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score3rdKillCountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score4thKillCountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score5thKillCountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Score6thKillCountTextBlock;
};
