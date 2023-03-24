#pragma once

#include "CoreMinimal.h"
#include "GameLobbyWeaponSelectWidget.h"
#include "GameLobbyCharacterSelectWidget.generated.h"


//������ ĳ���Ͱ� ����Ǿ��� �� �۵��ϴ� ��������Ʈ, �ش� ĳ������ �̸��� �Ѱ��ݴϴ�
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
