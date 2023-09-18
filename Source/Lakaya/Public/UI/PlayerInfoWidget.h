#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/Team.h"
#include "PlayerInfoWidget.generated.h"

UCLASS()
class LAKAYA_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlayerInfoWidget(const FObjectInitializer& ObjectInitializer);
	void SetPlayerName(const FString& NewName);
	void SetCharacterName(const FName& NewName);
	void SetTeam(const ETeam& NewTeam);
	
protected:
	virtual void NativeConstruct() override;

private:
	TObjectPtr<class UTextBlock> PlayerNameText;
	TObjectPtr<UTextBlock> CharacterNameText;

	TObjectPtr<class UImage> CharacterPortraitImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> CharacterPortraitMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETeam, TObjectPtr<UTexture2D>> BackgroundTextureMap;

	FText PlayerName;
};
