#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DetailResultElementWidget.generated.h"

UCLASS()
class LAKAYA_API UDetailResultElementWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
public:
	void SetElementWidget(const ALakayaBasePlayerState* NewPlayerState, const uint8 Index);

	// 플레이어 이름을 설정합니다.
	void SetPlayerName(UTextBlock* NewTextBlock, const FText& PlayerName) const;
	void SetPlayerName(UTextBlock* NewTextBlock, const FString& PlayerName) const;

	// 누적 점수를 업데이트합니다.
	void SetTotalScore(UTextBlock* NewTextBlock, const uint16& ScoreCount) const;

	// 누적 점령 성공 횟수를 업데이트합니다.
	void SetSuccessCaptureCount(UTextBlock* NewTextBlock, const uint16& CaptureCount) const;
	
	// 누적 킬 횟수를 업데이트합니다.
	void SetKillCount(UTextBlock* NewTextBlock, const uint16& KillCount) const;

	// 누적 사망 횟수를 업데이트합니다.
	void SetDeathCount(UTextBlock* NewTextBlock, const uint16& DeathCount) const;

public:
#pragma region Widget
	TWeakObjectPtr<UCanvasPanel> AF_Panel;
	TWeakObjectPtr<UCanvasPanel> AS_Panel;
	TWeakObjectPtr<UCanvasPanel> AT_Panel;
	TWeakObjectPtr<UCanvasPanel> PF_Panel;
	TWeakObjectPtr<UCanvasPanel> PS_Panel;
	TWeakObjectPtr<UCanvasPanel> PT_Panel;
	
	TWeakObjectPtr<UImage> AF_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> AF_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> AF_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> AF_Score_Text;
	TWeakObjectPtr<UTextBlock> AF_Occupation_Text;
	TWeakObjectPtr<UTextBlock> AF_Kill_Text;
	TWeakObjectPtr<UTextBlock> AF_Death_Text;
	
	TWeakObjectPtr<UImage> AS_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> AS_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> AS_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> AS_Score_Text;
	TWeakObjectPtr<UTextBlock> AS_Occupation_Text;
	TWeakObjectPtr<UTextBlock> AS_Kill_Text;
	TWeakObjectPtr<UTextBlock> AS_Death_Text;
	
	TWeakObjectPtr<UImage> AT_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> AT_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> AT_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> AT_Score_Text;
	TWeakObjectPtr<UTextBlock> AT_Occupation_Text;
	TWeakObjectPtr<UTextBlock> AT_Kill_Text;
	TWeakObjectPtr<UTextBlock> AT_Death_Text;
	
	TWeakObjectPtr<UImage> PF_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> PF_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> PF_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> PF_Score_Text;
	TWeakObjectPtr<UTextBlock> PF_Occupation_Text;
	TWeakObjectPtr<UTextBlock> PF_Kill_Text;
	TWeakObjectPtr<UTextBlock> PF_Death_Text;
	
	TWeakObjectPtr<UImage> PS_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> PS_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> PS_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> PS_Score_Text;
	TWeakObjectPtr<UTextBlock> PS_Occupation_Text;
	TWeakObjectPtr<UTextBlock> PS_Kill_Text;
	TWeakObjectPtr<UTextBlock> PS_Death_Text;
	
	TWeakObjectPtr<UImage> PT_Portrait_Rena_Image;
	TWeakObjectPtr<UImage> PT_Portrait_Wazi_Image;
	TWeakObjectPtr<UTextBlock> PT_PlayerName_Text;
	TWeakObjectPtr<UTextBlock> PT_Score_Text;
	TWeakObjectPtr<UTextBlock> PT_Occupation_Text;
	TWeakObjectPtr<UTextBlock> PT_Kill_Text;
	TWeakObjectPtr<UTextBlock> PT_Death_Text;
	
	TArray<TWeakObjectPtr<UCanvasPanel>> Panels;
	TArray<TWeakObjectPtr<UImage>> Portrait_Rena_Images;
	TArray<TWeakObjectPtr<UImage>> Portrait_Wazi_Images;
	TArray<TWeakObjectPtr<UTextBlock>> PlayerName_Texts;
	TArray<TWeakObjectPtr<UTextBlock>> Score_Texts;
	TArray<TWeakObjectPtr<UTextBlock>> Occupation_Texts;
	TArray<TWeakObjectPtr<UTextBlock>> Kill_Texts;
	TArray<TWeakObjectPtr<UTextBlock>> Death_Texts;
#pragma endregion 
};