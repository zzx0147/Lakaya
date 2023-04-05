#define DO_CHECK 1

#include "UI/GamePlayKillLogWidget.h"

#include "Character/DamageableCharacter.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "UI/KillLogElement.h"

void UGamePlayKillLogWidget::UpdateKillLogWidget(ADamageableCharacter* Character)
{
	if (Character) Character->OnKillCharacterNotify.AddUObject(this, &UGamePlayKillLogWidget::OnKillCharacterNotify);
	else UE_LOG(LogInit, Error, TEXT("SetupKillLogWidget::Character was nullptr!"));
}

UGamePlayKillLogWidget::UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxElementCount = 3;

	static const ConstructorHelpers::FClassFinder<UKillLogElement> ElementFinder(
		TEXT("/Game/Blueprints/UMG/WBP_KillLogElement"));

	KillLogClass = ElementFinder.Class;
	if (!KillLogClass) UE_LOG(LogInit, Error, TEXT("Fail to find ElementClass!"));
}

void UGamePlayKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KillLogBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("KillLog_Pan")));
	if (!KillLogBox) UE_LOG(LogInit, Fatal, TEXT("Fail to find KillLog_Pan!"));

	InitialChildCount = KillLogBox->GetChildrenCount();

	ElementPool.SetupObjectPool(MaxElementCount, [this]
	{
		auto Result = NewObject<UKillLogElement>(this, KillLogClass);
		Result->SetReturnFunction([this](UKillLogElement* Element)
		{
			ElementPool.ReturnObject(Element);
			KillLogBox->RemoveChild(Element);
		});
		return Result;
	});
}

void UGamePlayKillLogWidget::OnKillCharacterNotify(AController* KilledController, AActor* KilledActor,
                                                   AController* Instigator, AActor* Causer)
{
	if (KillLogBox->GetChildrenCount() >= InitialChildCount + MaxElementCount)
	{
		if (auto FirstKillLog = Cast<UKillLogElement>(KillLogBox->GetChildAt(InitialChildCount)))
			FirstKillLog->ExpireInstant();
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Fail to get FirstKillLog on OnKillCharacterNotify!"));
			return;
		}
	}
	auto Element = ElementPool.GetObject();
	KillLogBox->AddChild(Element);
	Element->SetKillLog(Cast<ADamageableCharacter>(Causer), Cast<ACharacter>(KilledActor));
}
