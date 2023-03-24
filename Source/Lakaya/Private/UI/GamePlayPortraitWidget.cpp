#define DO_CHECK 1

#include "UI/GamePlayPortraitWidget.h"
#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGamePlayPortraitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널 체크
#pragma region InitAndNullCheck

	CharacterPortraitImage = Cast<UImage>(GetWidgetFromName(TEXT("CharacterPortrait_Img")));

	CharacterPortraitTextureArray =
	{
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Citizen")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_GovernmentMan")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Gangster")),
	};

	check(CharacterPortraitImage != nullptr);
	for (auto temp : CharacterPortraitTextureArray) { check(temp != nullptr) }

#pragma endregion

	//기본 초상화를 0번(시민)으로 설정
	SetCharacterPortrait(0);
}

void UGamePlayPortraitWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeConstruct();
}

void UGamePlayPortraitWidget::SetCharacterPortrait(int32 CharacterNum)
{
	//캐릭터 넘버를 벗어나는 경우 종료
	if (CharacterNum < 0 || CharacterPortraitTextureArray.Num() <= CharacterNum) return;

	//캐릭터 넘버를 기준으로 배열에서 텍스처를 가져와 이미지를 변경
	CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitTextureArray[CharacterNum]);
}