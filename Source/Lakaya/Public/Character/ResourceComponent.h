// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceComponent();

	// 캐릭터의 생존 상태가 변경되면 호출됩니다. 서버, 클라이언트 모두에서 호출됩니다.
	virtual void OnAliveStateChanged(const bool& AliveState) { return; }
};
