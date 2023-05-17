#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHelpWidget.generated.h"

class UCanvasPanel;

//TODO: 아무 기능이 없는 클래스로 보여서 삭제해도 될거같습니다.
//플레이 UI중 도움말을 관리하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayHelpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayHelpWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UCanvasPanel* HelpBodyPanel;//도움말이 표기되는 패널입니다
};
