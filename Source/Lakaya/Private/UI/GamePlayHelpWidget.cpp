#define DO_CHECK 1

#include "GamePlayHelpWidget.h"
#include "Components/CanvasPanel.h"

UGamePlayHelpWidget::UGamePlayHelpWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGamePlayHelpWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//초기화 후 널 체크
#pragma region InitAndNullCheck

	HelpBodyPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("HelpBodyPanel")));
	check(HelpBodyPanel != nullptr);

#pragma endregion
}

void UGamePlayHelpWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
}
