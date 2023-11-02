// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabMinimapWidget.h"
#include "IndividualTabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIndividualTabMinimapWidget : public UTabMinimapWidget
{
	GENERATED_BODY()

public:
	virtual UImage* CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer) override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	virtual void UpdatePlayerPosition(const TWeakObjectPtr<ALakayaBasePlayerState>& NewPlayerState) override;
	virtual void UpdatePlayerPosition(const ETeam& NewTeam, const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState) override;

};