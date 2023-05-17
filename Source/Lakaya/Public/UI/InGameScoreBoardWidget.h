#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode//OccupationGameState.h"
#include "InGameScoreBoardWidget.generated.h"

//TODO: 안쓰이는 클래스면 지워도 될까요?
UCLASS()
class LAKAYA_API UInGameScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
};