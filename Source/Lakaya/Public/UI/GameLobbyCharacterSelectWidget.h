#pragma once

#include "CoreMinimal.h"
#include "UI/GameLobbyWeaponSelectWidget.h"
#include "GameLobbyCharacterSelectWidget.generated.h"


//선택한 캐릭터가 변경되었을 때 작동하는 델리게이트, 해당 캐릭터의 이름을 넘겨줍니다
DECLARE_MULTICAST_DELEGATE_OneParam(OnChangeSelectedCharacterDelegate, FName)

UCLASS()
class LAKAYA_API UGameLobbyCharacterSelectWidget : public UGameLobbyWeaponSelectWidget
{
	GENERATED_BODY()

public:
	UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ChangeSelectedCharacterImage(FString ImagePath);

	UFUNCTION()
	void OnClickedCharacter1Button();
	UFUNCTION()
	void OnClickedCharacter2Button();
	UFUNCTION()
	void OnClickedCharacter3Button();

	void SelectCharacter(int32 CharacterNum);

public:
	OnChangeSelectedCharacterDelegate OnChangeSelectedCharacter;

private:
	TArray<UButton*> CharacterButtonArray;
	TArray<UMaterialInterface*> CharacterRenderTargetMaterialArray;
	TArray<FName> CharacterNameArray;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectedCharacterImage;

	UButton* PrevCharacterButton;
};
