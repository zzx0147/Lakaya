#pragma once

#include "CoreMinimal.h"
#include "UI/GameLobbyWeaponSelectWidget.h"
#include "GameLobbyCharacterSelectWidget.generated.h"


//선택한 캐릭터가 변경되었을 때 작동하는 델리게이트, 해당 캐릭터의 이름을 넘겨줍니다
//param1 캐릭터 이름
DECLARE_EVENT_OneParam(UGameLobbyCharacterSelectWidget,OnChangeSelectedCharacterSigniture,const FName&)

//캐릭터 선택 UI 클래스
UCLASS()
class LAKAYA_API UGameLobbyCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void RegisterPlayer(APlayerState* PlayerState) { return; };

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
	OnChangeSelectedCharacterSigniture OnChangeSelectedCharacter;

private:
	TArray<TObjectPtr<UButton>> CharacterButtonArray;
	TArray<TObjectPtr<UMaterialInterface>> CharacterRenderTargetMaterialArray;
	TArray<FName> CharacterNameArray;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UImage> SelectedCharacterImage;

	TObjectPtr<UButton> PrevCharacterButton;
};