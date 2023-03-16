// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "GameLobbyWidget.generated.h"
/**
 * 
 */

//선택한 캐릭터가 변경되었을 때 작동하는 델리게이트, 해당 캐릭터의 이름을 넘겨줍니다
DECLARE_MULTICAST_DELEGATE_OneParam(OnChangeSelectedCharacterDelegate,FName)
//선택한 무기가 변경되었을 때 작동하는 델리게이트, 변경된 무기의 슬롯 넘버와 변경된 무기 이름을 반환합니다
DECLARE_MULTICAST_DELEGATE_TwoParams(OnChangeSelectedWeaponDelegate,int32,FName)

UCLASS()
class LAKAYA_API UGameLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameLobbyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

protected:
	//UFUNCTION(BlueprintCallable)
	void ChangeSelectedCharacterImage(FString ImagePath);

	UFUNCTION()
	void OnClickedCharacter1Button();
	UFUNCTION()
	void OnClickedCharacter2Button();
	UFUNCTION()
	void OnClickedCharacter3Button();

	void SelectCharacter(int32 CharacterNum);

	UFUNCTION()
	void OnPressedDraggableWeapon1Button();
	UFUNCTION()
	void OnPressedDraggableWeapon2Button();
	UFUNCTION()
	void OnPressedDraggableWeapon3Button();
	UFUNCTION()
	void OnPressedDraggableWeapon4Button();

	void OnPressDraggableWeaponButton(int WeaponNum);

	UFUNCTION()
	void OnReleasedDraggableWeapon1Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon2Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon3Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon4Button();

	void OnReleasedDraggableWeaponButton(int WeaponNum);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponSlotAtLocation(FVector2D AbsoluteLocation);

public:
	OnChangeSelectedCharacterDelegate OnChangeSelectedCharacter;
	OnChangeSelectedWeaponDelegate OnChangeSelectedWeapon;
protected:
	UPROPERTY(VisibleAnywhere)
	UImage* SelectedCharacterImage;

	UButton* PrevCharacterButton;

	UImage* DragImage;

	bool isDragOn = false;

	FVector2D CurrentMousePosisionWhileDrag;

	TArray<UButton*> CharacterButtonArray;
	TArray<UMaterialInterface*> CharacterRenderTargetMaterialArray;
	TArray<FName> CharacterNameArray;
	TArray<FName> WeaponNameArray;
	TArray<UImage*> WeaponSlotArray;
	TArray<UButton*> DraggableWeaponButtonArray;
	TArray<UTexture2D*> WeaponTextureArray;
	TArray<int32> SelectedWeaponArray;
};
