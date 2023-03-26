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
	
	UScoreBoardElement(const FObjectInitializer& ObjectInitializer);
public:
	void SetData(int8 Rank,const FText PlayerName, int8 Score); // 정보를 받아서 UI를 업데이트

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UTextBlock* RankText; //랭킹
	UTextBlock* PlayerNameText; //플레이어 이름
	UTextBlock* ScoreText; //점수
};