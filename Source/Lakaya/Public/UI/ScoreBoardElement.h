#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ScoreBoardElement.generated.h"

class UTextBlock;

//스코어 보드에서 캐릭터 한명의 데이터를 표기하는 UI(스코어 보드에서 한줄)
UCLASS()
class LAKAYA_API UScoreBoardElement : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	// 플레이어 이름을 설정합니다.
	virtual void SetPlayerName(const FText& PlayerName) const;
	virtual void SetPlayerName(const FString& PlayerName) const;

	// 누적 점수를 업데이트합니다.
	void SetTotalScore(const uint16& ScoreCount) const;

	// 누적 점령 성공 횟수를 업데이트합니다.
	void SetSuccessCaptureCount(const uint16& CaptureCount) const;
	
	// 누적 킬 횟수를 업데이트합니다.
	virtual void SetKillCount(const uint16& KillCount) const;

	// 누적 사망 횟수를 업데이트합니다.
	virtual void SetDeathCount(const uint16& DeathCount) const;
	
private:
	TWeakObjectPtr<UTextBlock> PlayerNameText;	 // 플레이어 이름
	TWeakObjectPtr<UTextBlock> ScoreText;	   	 // 점수
	TWeakObjectPtr<UTextBlock> CaptureText;		 // 점령 성공
	TWeakObjectPtr<UTextBlock> KillText;		 // 킬
	TWeakObjectPtr<UTextBlock> DeathText;		 // 데스

public:
	TWeakObjectPtr<UImage> Anti_BackGround_Image;			// Anti_뒷배경
	TWeakObjectPtr<UImage> Pro_BackGround_Image;			// Pro_뒷배경
	TWeakObjectPtr<UImage> Character_Rena_Image;		// Anti_레나 초상화
	TWeakObjectPtr<UImage> Character_Wazi_Image;		// Anti_와지 초상화
};