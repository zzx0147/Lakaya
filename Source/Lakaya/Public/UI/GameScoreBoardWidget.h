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
};