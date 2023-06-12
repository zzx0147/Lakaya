// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AGameLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGameLobbyPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* PawnToPossess) override;

public:
	void SetEnableExitShortcut(const bool& Enable);
	virtual void UnbindAllAndBindMenu( UEnhancedInputComponent* const& EnhancedInputComponent);

protected:
	/**
	 * @brief 이 플레이어 컨트롤러가 빙의중인 폰이 변경될 때 호출됩니다. 서버에서든 클라이언트에서든 모두 호출됩니다.
	 * @param ArgOldPawn 빙의중이었던 폰 객체입니다.
	 * @param NewPawn 새로 빙의한 폰 객체입니다.
	 */
	UFUNCTION()
	virtual void OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn) { return; }

	/**
	 * @brief 향상된 입력을 사용하는 우리 게임을 위해 선언된 함수입니다. SetupInputComponent에서 호출됩니다.
	 * @param EnhancedInputComponent 향상된 입력 컴포넌트 객체입니다. 이를 통해 인풋 바인딩이 가능합니다.
	 */
	virtual void SetupEnhancedInputComponent(class UEnhancedInputComponent* const& EnhancedInputComponent);


	/**
	 * @brief 향상된 입력을 사용하는 우리 게임을 위해 선언된 함수입니다. SetupInputComponent에서 호출됩니다.
	 * @param InputSubsystem 인풋 로컬 서브시스템 객체입니다. 이를 통해 인풋 맵핑 컨텍스트를 추가할 수 있습니다.
	 */
	virtual void SetupMappingContext(class UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem);

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> ExitLevel;
private:
	void ShowScoreBoard();
	void HideScoreBoard();
	void MenuHandler();

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputMappingContext* InterfaceInputContext;

	UPROPERTY(EditAnywhere, Category=Input)
	int8 InterfaceContextPriority;

	UPROPERTY(EditAnywhere, Category=Input)
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ShowScoreAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* HideScoreAction;

	bool bEnableExitShortcut;
};
