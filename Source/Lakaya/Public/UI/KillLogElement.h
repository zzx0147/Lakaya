// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogElement.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UKillLogElement : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	/**
	 * @brief 킬 로그의 내용을 셋업하고 킬 로그를 활성화합니다.
	 * @param Attacker Victim을 처치한 캐릭터입니다.
	 * @param Victim 처치당한 캐릭터입니다.
	 */
	void SetupKillLog(class ADamageableCharacter* Attacker, class ACharacter* Victim);

private:
	template <class T>
	void InitializeWidgetPtr(TWeakObjectPtr<T>& Ptr, const FName& WidgetName)
	{
		Ptr = Cast<T>(GetWidgetFromName(WidgetName));
		if (Ptr.IsStale()) UE_LOG(LogInit, Error, TEXT("Fail to find %s!"), *WidgetName.ToString());
	}

	void ShowTimerCallback();

	TWeakObjectPtr<class UTextBlock> VictimTextBlock;
	TWeakObjectPtr<UTextBlock> AttackerTextBlock;
	TWeakObjectPtr<class UImage> WeaponImage;
	FTimerHandle ShowTimer;

	UPROPERTY(EditAnywhere)
	float ShowingTime;
};
