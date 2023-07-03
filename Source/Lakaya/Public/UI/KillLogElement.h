#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/PlayerTeam.h"
#include "KillLogElement.generated.h"

UCLASS()
class LAKAYA_API UKillLogElement : public UUserWidget
{
	GENERATED_BODY()

public:
	UKillLogElement(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	inline void SetReturnFunction(std::function<void(UKillLogElement*)> ReturnFunction) { Return = ReturnFunction; }

	/**
	 * @brief 킬 로그의 내용을 셋업하고 킬 로그를 활성화합니다.
	 * @param Attacker Victim을 처치한 캐릭터입니다.
	 * @param Victim 처치당한 캐릭터입니다.
	 */
	void SetKillLog(APlayerState* Attacker, APlayerState* Victim);

	// 킬 로그를 즉시 만료시켜 비활성화 합니다.
	void ExpireInstant();

private:
	template <class T>
	void InitializeWidgetPtr(TWeakObjectPtr<T>& Ptr, const FName& WidgetName)
	{
		Ptr = Cast<T>(GetWidgetFromName(WidgetName));
		if (Ptr.IsStale()) UE_LOG(LogInit, Error, TEXT("Fail to find %s!"), *WidgetName.ToString());
	}

	void ExpireKillLog();

	TWeakObjectPtr<class UTextBlock> VictimTextBlock;
	TWeakObjectPtr<UTextBlock> AttackerTextBlock;
	TWeakObjectPtr<class UImage> WeaponImage;
	TWeakObjectPtr<UImage> AttackerImage;
	TWeakObjectPtr<UImage> VictimImage;
	TWeakObjectPtr<UImage> BackgroundImage;

	
	FTimerHandle ShowTimer;
	std::function<void(UKillLogElement*)> Return;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<class UTexture2D>> WeaponImageMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TObjectPtr<class UTexture2D>> CharacterImageMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<EPlayerTeam, TObjectPtr<class UTexture2D>> BackgroundImageMap;

	UPROPERTY(EditAnywhere)
	float ShowingTime;
};
