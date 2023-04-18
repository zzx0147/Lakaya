#define DO_CHECK 1

#include "UI/GamePlayBulletWidget.h"

#include "Character/BulletComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/TextBlock.h"


void UGamePlayBulletWidget::BindCharacter(ACharacter* const& Character)
{
	Super::BindCharacter(Character);
	if (const auto CastedCharacter = Cast<ALakayaBaseCharacter>(Character))
	{
		BindBulletComponent(CastedCharacter->GetResourceComponent());
		CastedCharacter->OnResourceChanged.AddUObject(this, &UGamePlayBulletWidget::BindBulletComponent);
	}
}

bool UGamePlayBulletWidget::UnbindCharacter(ACharacter* const& Character)
{
	OnChangeRemainBullets(0);
	OnChangeMagazineCapacity(0);
	if (const auto CastedCharacter = Cast<ALakayaBaseCharacter>(Character))
		CastedCharacter->OnResourceChanged.RemoveAll(this);
	return false;
}

void UGamePlayBulletWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	RemainBulletsText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RemainBullets_Text")));
	MagazineCapacityText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MagazineCapacity_Text")));

	check(RemainBulletsText != nullptr);
	check(MagazineCapacityText != nullptr);

#pragma endregion
}

void UGamePlayBulletWidget::BindBulletComponent(UResourceComponent* const& ResourceComponent)
{
	if (const auto BulletComponent = Cast<UBulletComponent>(ResourceComponent))
	{
		OnChangeRemainBullets(BulletComponent->GetBullets());
		OnChangeMagazineCapacity(BulletComponent->GetMaxBullets());

		BulletComponent->OnBulletsChanged.AddUObject(this, &UGamePlayBulletWidget::OnChangeRemainBullets);
		BulletComponent->OnMaxBulletsChanged.AddUObject(this, &UGamePlayBulletWidget::OnChangeMagazineCapacity);
	}
}

void UGamePlayBulletWidget::OnChangeRemainBullets(const uint16& NewRemainBullets) const
{
	//업데이트된 총알 갯수를 저장하고 텍스트로 표기
	RemainBulletsText->SetText(FText::AsNumber(NewRemainBullets));
}

void UGamePlayBulletWidget::OnChangeMagazineCapacity(const uint16& NewMagazineCapacity) const
{
	//업데이트된 최대 총알 갯수를 저장하고 텍스트로 표기(앞에 /를 붙여서 표기)
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMagazineCapacity)));
}
