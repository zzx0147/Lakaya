#define DO_CHECK 1

#include "UI/DirectionalIndicatorElement.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Character/DamageableCharacter.h"
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
		FVector ForwardVector = MyPosition->GetForwardVector();//카메라의 포워드 벡터
		ForwardVector = FVector(ForwardVector.X, ForwardVector.Y, 0.0f);//높이는 버린다

		FVector ToTargetVector = TargetPosition - MyPosition->GetComponentLocation();//카메라로부터 적 위치를 가르키는 벡터
		ToTargetVector = FVector(ToTargetVector.X, ToTargetVector.Y, 0.0f);//높이는 버린다

		double Degrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector.GetSafeNormal(),ToTargetVector.GetSafeNormal())));
		
		FVector CrossProduct = FVector::CrossProduct(ForwardVector, ToTargetVector);
		
		DirectionalIndicatorImage->SetRenderTransformAngle(CrossProduct.Z < 0 ? -Degrees : Degrees);
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