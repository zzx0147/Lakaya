#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode//OccupationGameState.h"
#include "InGameScoreBoardWidget.generated.h"

UCLASS()
class LAKAYA_API UInGameScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

public:
	void BindPlayerScore(class ACollectorPlayerState* CollectorPlayerState);
	void OnChangePlayerScore(int32 NewScore, int32 NewKills, int32 NewOcuSuc, int32 NewOwnObjectNum);

	// TArray<ACollectorPlayerState*> PlayerStatesArray;

	int32 MyIndex;

#pragma region Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_01;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_02;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_03;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_01;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_02;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_03;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_01_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_02_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_03_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_01_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_02_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_03_TotalScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_01_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_02_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_03_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_01_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_02_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_03_Kill;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_01_OcuSuc;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_02_OcuSuc;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* A_Player_03_OcuSuc;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_01_OcuSuc;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_02_OcuSuc;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* B_Player_03_OcuSuc;
#pragma endregion

private:
	AOccupationGameState* OccupationGameState;
	ACollectorPlayerState* CollectorPlayerState;
};