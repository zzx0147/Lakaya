#define DO_CHECK 1

#include "UI/GamePlayKillLogWidget.h"

#include "Components/VerticalBox.h"
#include "GameMode/OccupationGameState.h"
#include "UI/KillLogElement.h"


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
		const auto Result = NewObject<UKillLogElement>(this, KillLogClass);
		Result->SetReturnFunction([this](UKillLogElement* Element)
		{
			ElementPool.ReturnObject(Element);
			--ShownElementCount;
		});
		KillLogBox->AddChildToVerticalBox(Result);
		Result->SetVisibility(ESlateVisibility::Collapsed);
		return Result;
	});

	if (const auto GameState = GetWorld()->GetGameState<AOccupationGameState>())
		GameState->OnKillCharacterNotify.AddUObject(this, &UGamePlayKillLogWidget::OnKillCharacterNotify);
}

void UGamePlayKillLogWidget::OnKillCharacterNotify(AController* KilledController, AActor* KilledActor,
                                                   AController* Instigator, AActor* Causer)
{
	UKillLogElement* Element;
	if (ShownElementCount < MaxElementCount)
	{
		Element = ElementPool.GetObject();
		++ShownElementCount;
	}
	else Element = Cast<UKillLogElement>(KillLogBox->GetChildAt(InitialChildCount));

	KillLogBox->ShiftChild(KillLogBox->GetChildrenCount(), Element);
	// Element->SetKillLog(Cast<ADamageableCharacter>(Causer), Cast<ACharacter>(KilledActor));
}
