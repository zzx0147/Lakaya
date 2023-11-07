// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupyExpressWidget.h"

void UOccupyExpressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AntiAreaInImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiAreaInImage is null."));
	if (CenterAreaInImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("CenterAreaInImage is null."));
	if (ProAreaInImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProAreaInImage is null."));

	if (AntiAreaBar == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiAreaProgressBar is null."));
	if (CenterAreaBar == nullptr) UE_LOG(LogTemp, Warning, TEXT("CenterAreaProgressBar is null."));
	if (ProAreaBar == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProAreaProgressBar is null."));
}