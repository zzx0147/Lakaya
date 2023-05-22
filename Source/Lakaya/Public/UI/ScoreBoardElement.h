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
	UE_DEPRECATED(5.1, "이 함수는 더이상 사용되지 않습니다. 대신 다른 setter를 사용하세요.")
	void SetData(int8 Rank,const FText& PlayerName, int8 Score) const; // 정보를 받아서 UI를 업데이트

	// 플레이어 이름을 설정합니다.
	void SetPlayerName(const FText& PlayerName) const;
	void SetPlayerName(const FString& PlayerName) const;

	// 누적 점수를 업데이트합니다.
	void SetScoreCount(const uint32& ScoreCount) const;
	
	// 누적 킬 횟수를 업데이트합니다.
	void SetKillCount(const uint16& KillCount) const;

	// 누적 점령 성공 횟수를 업데이트합니다.
	void SetCaptureCount(const uint16& CaptureCount) const;

	// 누적 사망 횟수를 업데이트합니다.
	void SetDeathCount(const uint16& DeathCount) const;
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