#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DetailResultElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UDetailResultElementWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
public:
	// 플레이어 이름을 설정합니다.
	void SetPlayerName(const FText& PlayerName) const;
	void SetPlayerName(const FString& PlayerName) const;

	// 누적 점수를 업데이트합니다.
	void SetTotalScore(const uint16& ScoreCount) const;

	// 누적 점령 성공 횟수를 업데이트합니다.
	void SetSuccessCaptureCount(const uint16& CaptureCount) const;
	
	// 누적 킬 횟수를 업데이트합니다.
	void SetKillCount(const uint16& KillCount) const;

	// 누적 사망 횟수를 업데이트합니다.
	void SetDeathCount(const uint16& DeathCount) const;

	void SetAntiTotalBoxElementImage() const;
	void SetProTotalBoxElementImage() const;

	void SetPortraitRenaImage() const;
	void SetPortraitWaziImage() const;
	
private:
	TWeakObjectPtr<UImage> AntiTotalBoxElementImage;
	TWeakObjectPtr<UImage> ProTotalBoxElementImage;

	TWeakObjectPtr<UTextBlock> PlayerNameText;

	TWeakObjectPtr<UImage> PortraitRenaImage;
	TWeakObjectPtr<UImage> PortraitWaziImage;

	TWeakObjectPtr<UTextBlock> ScoreText;
	TWeakObjectPtr<UTextBlock> OccupationText;
	TWeakObjectPtr<UTextBlock> KillText;
	TWeakObjectPtr<UTextBlock> DeathText;
};