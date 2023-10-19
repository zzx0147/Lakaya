// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DataAsset.h"
#include "LakayaInputContext.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaInputContext : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 이 데이터 에셋의 컨텍스트를 추가합니다. 이미 컨텍스트가 서브 시스템에 존재하면 추가되지 않습니다.
	 * @param InputSubsystem 컨텍스트를 추가할 대상 서브시스템입니다.
	 */
	FORCEINLINE void AddMappingContext(UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
	{
		if (ensure(IsValid(InputSubsystem)) && !InputSubsystem->HasMappingContext(Context))
		{
			InputSubsystem->AddMappingContext(Context, Priority);
		}
	}

	/**
	 * @brief 이 데이터 에셋의 컨텍스트를 서브시스템에서 제거합니다.
	 * @param InputSubsystem 컨텍스트를 제거할 대상 서브시스템입니다.
	 */
	FORCEINLINE void RemoveMappingContext(UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
	{
		if (ensure(IsValid(InputSubsystem)) && InputSubsystem->HasMappingContext(Context))
		{
			InputSubsystem->RemoveMappingContext(Context);
		}
	}

private:
	/** 기본 컨텍스트를 지정합니다. */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> Context;

	/** 컨텍스트의 중요도를 지정합니다. */
	UPROPERTY(EditAnywhere)
	int32 Priority;
};
