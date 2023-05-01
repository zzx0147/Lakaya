#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameScoreBoardWidget.generated.h"

class UScoreBoardElement;
class UCanvasPanel;

//스코어 보드를 관리하는 클래스
UCLASS()
class LAKAYA_API UGameScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스코어보드에 정보를 표시할 플레이어를 등록합니다.
	virtual void RegisterPlayer(APlayerState* PlayerState) { return; }

protected:
	virtual void NativeConstruct() override;

private:
	UE_DEPRECATED(5.1, "스코어보드 클래스 구조가 변경됨에 따라 더이상 사용되지 않습니다.")
	void InitScoreBoardElements(int8 ElementsNum); //스코어 보드에 ElementsNum 갯수만큼 ScoreBoardElement를 생성하고 배열에 저장합니다

	UE_DEPRECATED(5.1, "스코어보드 클래스 구조가 변경됨에 따라 더이상 사용되지 않습니다.")
	TArray<UScoreBoardElement*> ScoreBoardElementArray; //스코어보드 Element를 저장하는 배열

	UE_DEPRECATED(5.1, "스코어보드 클래스 구조가 변경됨에 따라 더이상 사용되지 않습니다.")
	UCanvasPanel* ScoreBoardPanel; //Element가 그려지는 패널
};
