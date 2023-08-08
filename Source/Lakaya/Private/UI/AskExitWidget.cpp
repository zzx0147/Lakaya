#include "UI/AskExitWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

bool UAskExitWidget::Initialize()
{
	bool bResult = Super::Initialize();
	if (!bResult) return false;

	// if (AskExit_Yes_Button != nullptr)
	// 	AskExit_Yes_Button->OnClicked.AddDynamic(this, &UAskExitWidget::QuitGame);
	//
	// if (AskExit_No_Button != nullptr)
	// 	AskExit_No_Button->OnClicked.AddDynamic(this, &UAskExitWidget::BackLobby);
	
	return true;
}

void UAskExitWidget::QuitGame()
{
	if (auto World = GetWorld())
	{
		if (auto PlayerController = World->GetFirstPlayerController())
		{
			UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
		}
	}
}

void UAskExitWidget::BackLobby()
{
	SetVisibility(ESlateVisibility::Hidden);
}