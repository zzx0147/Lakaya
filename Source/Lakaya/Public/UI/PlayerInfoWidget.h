#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

UCLASS()
class LAKAYA_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPlayerInfoWidget(const FObjectInitializer& ObjectInitializer);
	void SetPlayerName(FString NewName);
	void SetCharacterName(const FName& NewName);
	
protected:
	virtual void NativeConstruct() override;

private:
	TObjectPtr<class UTextBlock> PlayerNameText;
	TObjectPtr<UTextBlock> CharacterNameText;
	TObjectPtr<class UImage> CharacterPortraitImage;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName,TObjectPtr<class UTexture2D>> CharacterPortraitMap;

	FText PlayerName;
};
