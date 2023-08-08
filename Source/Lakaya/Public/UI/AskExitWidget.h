#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AskExitWidget.generated.h"

UCLASS()
class LAKAYA_API UAskExitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void QuitGame();

	UFUNCTION(BlueprintCallable)
	void BackLobby();
protected:
	// UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	// TWeakObjectPtr<class UButton> AskExit_Yes_Button;
	//
	// UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	// TWeakObjectPtr<UButton> AskExit_No_Button;
};