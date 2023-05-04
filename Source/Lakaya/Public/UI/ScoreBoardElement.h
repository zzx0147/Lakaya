#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreBoardElement.generated.h"

class UTextBlock;

//스코어 보드에서 캐릭터 한명의 데이터를 표기하는 UI(스코어 보드에서 한줄)
UCLASS()
class LAKAYA_API UScoreBoardElement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UE_DEPRECATED(5.1, "이 함수는 더이상 사용되지 않습니다. 대신 다른 setter를 사용하세요.")
	void SetData(int8 Rank,const FText& PlayerName, int8 Score); // 정보를 받아서 UI를 업데이트

	// 누적 킬 횟수를 업데이트합니다.
	void SetKillCount(const uint16& KillCount);

	// 누적 사망 횟수를 업데이트합니다.
	void SetDeathCount(const uint16& DeathCount);

	// 플레이어 이름을 설정합니다.
	void SetPlayerName(const FText& PlayerName);
	void SetPlayerName(const FString& PlayerName);

protected:
	virtual void NativeConstruct() override;

private:
	//TODO: 아래의 두 텍스트블럭은 변경되는 UI 기획서에서 여전히 사용되는지 여부를 확인해봐야 합니다.
	UTextBlock* RankText; //랭킹
	UTextBlock* ScoreText; //점수

	TWeakObjectPtr<UTextBlock> PlayerNameText; //플레이어 이름
	TWeakObjectPtr<UTextBlock> KillText;
	TWeakObjectPtr<UTextBlock> DeathText;
};