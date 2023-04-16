#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "GameMode/OccupationGameState.h"
#include "GameScoreBoardWidget.generated.h"

class UScoreBoardElement;
class UCanvasPanel;

//스코어 보드를 관리하는 클래스
UCLASS()
class LAKAYA_API UGameScoreBoardWidget : public UMatchStateWidget
{
	GENERATED_BODY()

public:
	virtual bool OnMatchStart() override;

protected:
	virtual void NativeConstruct() override;

private:
	void InitScoreBoardElements(int8 ElementsNum); //스코어 보드에 ElementsNum 갯수만큼 ScoreBoardElement를 생성하고 배열에 저장합니다

	TArray<UScoreBoardElement*> ScoreBoardElementArray; //스코어보드 Element를 저장하는 배열
	UCanvasPanel* ScoreBoardPanel; //Element가 그려지는 패널
};
