#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayPortraitWidget.generated.h"

class UImage;
class UTexture2D;

USTRUCT(BlueprintType)
struct FPortraitImageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> CharacterImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> WeaponImage;
};

//플레이 UI 중 캐릭터 초상화를 관리하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayPortraitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer);

	// 캐릭터 이름을 통해 초상화를 업데이트합니다.
	void ChangePortrait(const FName& Key);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> CharacterPortraitImage; //캐릭터 초상화를 표기하는 Image 위젯

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> WeaponPortraitImage;

	UPROPERTY(EditAnywhere)
	TMap<FName, FPortraitImageData> PortraitTextureMap; //표기될 초상화 텍스쳐 맵
};
