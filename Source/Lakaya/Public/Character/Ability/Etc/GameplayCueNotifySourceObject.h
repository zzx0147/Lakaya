// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayCueNotifySourceObject.generated.h"

/**
 * 게임플레이 큐 노티파이 액터의 각 인스턴스는 타겟, 게임플레이 큐, 인스티게이터, 소스 오브젝트를 통해서 유니크한 인스턴스를 생성할 수 있습니다.
 * 이 클래스는 유니크한 게임플레이 큐 노티파이 액터 인스턴스가 필요하기는 하지만, 소스 오브젝트로 등록할 오브젝트는 딱히 없을 때 사용하기 위해 만들어졌습니다.
 */
UCLASS()
class LAKAYA_API UGameplayCueNotifySourceObject : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UObject* GetGameplayCueNotifySourceObject(const int32& Index);

	UGameplayCueNotifySourceObject();

private:
	UPROPERTY()
	TArray<UObject*> SourceObjects;
};
