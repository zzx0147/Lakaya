#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/IndividualGameState.h"
#include "GameMode/OccupationGameState.h"
#include "GameScoreBoardWidget.generated.h"

class UScoreBoardElement;
class UCanvasPanel;

//스코어 보드를 관리하는 클래스
UCLASS()
class LAKAYA_API UGameScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetGameScoreBoardWidget(EOccupationGameState ChangeGameState);
	
public:
	TArray<UScoreBoardElement*> ScoreBoardElementArray;//스코어보드 Element를 저장하는 배열
	UCanvasPanel* ScoreBoardPanel;//Element가 그려지는 패널

public:
	void InitScoreBoardElements(int8 ElementsNum); //스코어 보드에 ElementsNum 갯수만큼 ScoreBoardElement를 생성하고 배열에 저장합니다
};