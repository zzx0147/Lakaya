#define DO_CHECK 1

#include "UI/DirectionalIndicatorElement.h"
#include "Components/Image.h"

UDirectionalIndicatorElement::UDirectionalIndicatorElement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsIndicating = false;
}

void UDirectionalIndicatorElement::NativeConstruct()
{
	Super::NativeConstruct();
	
	
#pragma region InitAndNullCheck
	DirectionalIndicatorImage = Cast<UImage>(GetWidgetFromName(TEXT("DirectionalIndicator_Img")));

	check(DirectionalIndicatorImage != nullptr);
#pragma endregion


	DirectionalIndicatorImage->SetVisibility(ESlateVisibility::Hidden);
}

void UDirectionalIndicatorElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsIndicating)
	{
		//Cast<USceneComponent>(MyPosition);
	}
}

void UDirectionalIndicatorElement::IndicateStart(USceneComponent* NewMyPosition, FVector NewTargetPosition, float Time)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleIndicate);

	bIsIndicating = true;
	MyPosition = NewMyPosition;
	TargetPosition = NewTargetPosition;
	DirectionalIndicatorImage->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(TimerHandleIndicate, this, &UDirectionalIndicatorElement::IndicateStop, Time, false);
}

void UDirectionalIndicatorElement::IndicateStop()
{
	bIsIndicating = false;
	MyPosition = nullptr;
	TargetPosition = FVector::Zero();
	DirectionalIndicatorImage->SetVisibility(ESlateVisibility::Hidden);
}
