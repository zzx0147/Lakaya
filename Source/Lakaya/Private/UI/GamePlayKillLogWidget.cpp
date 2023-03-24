#define DO_CHECK 1

#include "GamePlayKillLogWidget.h"
#include "KillLogElement.h"

UGamePlayKillLogWidget::UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGamePlayKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널 체크
#pragma region InitAndNullCheck

	KillLogElementArray =
	{
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement1"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement2"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement3")))
	};

	for (auto temp : KillLogElementArray) { check(temp != nullptr) }

#pragma endregion
}

void UGamePlayKillLogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
