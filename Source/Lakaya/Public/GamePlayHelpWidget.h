#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHelpWidget.generated.h"

class UCanvasPanel;

//�÷��� UI�� ������ �����ϴ� Ŭ�����Դϴ�
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
	UCanvasPanel* HelpBodyPanel;//������ ǥ��Ǵ� �г��Դϴ�
};
