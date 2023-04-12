#define DO_CHECK 1

#include "UI/DirectionalDamageIndicator.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UDirectionalDamageIndicator::UDirectionalDamageIndicator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FClassFinder<UDirectionalIndicatorElement> DirectionalIndicatorClassFinder(
		TEXT("/Game/Blueprints/UMG/WBP_DirectionalIndicatorElement.WBP_DirectionalIndicatorElement_C"));

	check(DirectionalIndicatorClassFinder.Class != nullptr);

	IndicatorElementClass = DirectionalIndicatorClassFinder.Class;
}

void UDirectionalDamageIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	IndicatorPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Indicator_Pan")));
	if (IndicatorPanel == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("IndicatorPanel is null."));
		return;
	}
}

void UDirectionalDamageIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDirectionalDamageIndicator::IndicateStart(const FName& CauserName, USceneComponent* MyPosition, FVector DamageCursorPosition, float time)
{
	UDirectionalIndicatorElement* result = nullptr;
	UDirectionalIndicatorElement** resultPtr = (IndicatorMap.Find(CauserName));
	if (resultPtr != nullptr) result = *resultPtr;


	if (result == nullptr)
	{
		result = CreateWidget<UDirectionalIndicatorElement>(this, IndicatorElementClass);

		if (result == nullptr) return;

		IndicatorMap.Add(CauserName, result);
		IndicatorPanel->AddChild(result);

		UCanvasPanelSlot* tempSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(result);
		tempSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		tempSlot->SetAlignment(FVector2d(0.5f, 0.5f));
		tempSlot->SetPosition(FVector2d(0.0f, 0.0f));
		tempSlot->SetOffsets(FMargin(0.0f));
	}
	result->IndicateStart(MyPosition, DamageCursorPosition, time);
}
