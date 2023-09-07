#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameLobbyCharacterSelectWidget.generated.h"

//선택한 캐릭터가 변경되었을 때 작동하는 델리게이트, 해당 캐릭터의 이름을 넘겨줍니다
//param1 캐릭터 이름
DECLARE_EVENT_OneParam(UGameLobbyCharacterSelectWidget, OnChangeSelectedCharacterSignature, const FName&)

//캐릭터 선택 UI 클래스
UCLASS()
class LAKAYA_API UGameLobbyCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	virtual void RegisterPlayer(APlayerState* PlayerState) { return; }

	// 캐릭터가 선택되면 자동으로 위젯이 가려질지 여부를 선택합니다. 
	void EnableAutoHide(const bool& IsEnabled);

	void SetShortcutEnabled(const bool& Enable);

	void ToggleVisibility();

protected:
	//버튼에 바인딩되는 함수들은 UFUNTION을 사용해야함
	UFUNCTION()
	void OnClickedCharacter1Button();
	UFUNCTION()
	void OnClickedCharacter2Button();
	UFUNCTION()
	void OnClickedCharacter3Button();

	void SelectCharacter(const uint8& CharacterNum);

public:
	OnChangeSelectedCharacterSignature OnChangeSelectedCharacter;

protected:
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* ShortcutContext;

	UPROPERTY(EditDefaultsOnly)
	int32 ShortcutPriority;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* ToggleAction;

	UPROPERTY(EditDefaultsOnly)
	bool bAutoShortcutEnable;

private:
	TArray<TObjectPtr<class UButton>> CharacterButtonArray;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMaterialInterface>> CharacterRenderTargetMaterialArray;
	TArray<FName> CharacterNameArray;

	UPROPERTY(EditDefaultsOnly, meta=( MultiLine="true" ))
	TMap<FName, FText> CharacterIntroductionMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> GunTextureMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, uint32> MagazineMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture2D>> CharacterBackgroundTextureMap;
	
	
	TObjectPtr<class UImage> GunImage;

	TObjectPtr<class UImage> CharacterBackgroundImage;
	
	TObjectPtr<UImage> SelectedCharacterImage;

	TObjectPtr<UButton> PrevCharacterButton;

	TObjectPtr<class UTextBlock> MagazineInfoText;

	FText MagazineTextFormat;

	TObjectPtr<class URichTextBlock> IntroductionText;

	TObjectPtr<class UPlayerInfoWidget> PlayerInfoWidget;

	bool bAutoHide;
};
