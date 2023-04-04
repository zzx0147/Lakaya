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
	static const ConstructorHelpers::FClassFinder<UKillLogElement> ElementFinder(
		TEXT("/Game/Blueprints/UMG/WBP_KillLogElement"));

	ElementClass = ElementFinder.Class;
	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("Fail to find ElementClass!"));
}

void UGamePlayKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KillLogBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("KillLog_Pan")));
	if (!KillLogBox) UE_LOG(LogInit, Error, TEXT("Fail to find KillLog_Pan!"));
}

void UGamePlayKillLogWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ElementPool.SetupObjectPool(3, [this]
	{
		auto Result = NewObject<UKillLogElement>(this, ElementClass);
		Result->SetReturnFunction([this,Result] { ElementPool.ReturnObject(Result); });
		return Result;
	});
}

void UGamePlayKillLogWidget::OnKillCharacterNotify(AController* KilledController, AActor* KilledActor,
                                                   AController* Instigator, AActor* Causer)
{
	auto Element = ElementPool.GetObject();
	KillLogBox->AddChild(Element);
	Element->SetKillLog(Cast<ADamageableCharacter>(Causer), Cast<ACharacter>(KilledActor));
}
