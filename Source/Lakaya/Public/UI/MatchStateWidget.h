// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UMatchStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 매치가 시작되고 호출됩니다.
	 * @return 이 위젯이 계속 남아있을지 여부입니다. true면 남고, false이면 제거됩니다.
	 */
	virtual bool OnMatchStart() { return false; }

	/**
	 * @brief 매치가 종료되고 씬을 떠나기 전에 호출됩니다.
	 * @return 이 위젯이 계속 남아있을지 여부입니다. true면 남고, false이면 제거됩니다.
	 */
	virtual bool OnMatchEnding() { return false; }
};
