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

void UGamePlayKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널 체크
#pragma region InitAndNullCheck

	KillLogBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("KillLog_Pan")));
	if (!KillLogBox) UE_LOG(LogInit, Error, TEXT("Fail to find KillLog_Pan!"));

	KillLogElementArray =
	{
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement1"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement2"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement3")))
	};

	for (auto KillLogElement : KillLogElementArray)
	{
		check(KillLogElement != nullptr)
		KillLogElement->RemoveFromParent();
	}

#pragma endregion
}

void UGamePlayKillLogWidget::OnKillCharacterNotify(AController* KilledController, AActor* KilledActor,
                                                   AController* Instigator, AActor* Causer)
{
	auto Element = MakeFreshElement();
	Element->SetupKillLog(Cast<ADamageableCharacter>(Causer), Cast<ACharacter>(KilledActor));
}

UKillLogElement* UGamePlayKillLogWidget::MakeFreshElement()
{
	for (auto Element : KillLogElementArray)
	{
		if (Element->GetParent()) continue;
		else
		{
			KillLogBox->AddChild(Element);
			return Element;
		}
	}
	auto First = KillLogBox->GetChildAt(0);
	KillLogBox->RemoveChild(First);
	KillLogBox->AddChild(First);
	return Cast<UKillLogElement>(First);
}
