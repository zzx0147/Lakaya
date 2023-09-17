// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Occupation/Team.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "OccupationCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationCharacterSelectWidget : public UGameLobbyCharacterSelectWidget
{
	GENERATED_BODY()

public:
	explicit UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RegisterPlayer(APlayerState* PlayerState) override;

	void SetTeam(const ETeam& NewTeam);

protected:
	virtual void NativeConstruct() override;
	
	virtual void SelectCharacter(const uint8& CharacterNum);
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerInfoWidget> PlayerInfoWidgetClass;

	TObjectPtr<class UVerticalBox> PlayerInfoVerticalBox;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> AntiCharacterNameTextureMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> ProCharacterNameTextureMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> TeamInfoTextTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> CharacterSelectTextTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> GangrimButtonDisabledTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> RenaButtonDisabledTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> WaziButtonDisabledTextureMap;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TeamInfoTextImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterSelectTextImage;

	UPROPERTY()
	TArray<TObjectPtr<UPlayerInfoWidget>> OtherPlayerInfoArray;
	
	ETeam MyTeam;
};
