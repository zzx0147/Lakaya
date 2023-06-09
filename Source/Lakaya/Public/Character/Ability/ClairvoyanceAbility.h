#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/CharacterAbility.h"
#include "ClairvoyanceAbility.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UClairvoyanceAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UClairvoyanceAbility();
	
	virtual void OnAliveStateChanged(const bool& AliveState) override;
	UFUNCTION(BlueprintCallable)
	void SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial);
	void CreateClairvoyanceWidget(APlayerController* PlayerController);
	
protected:
	virtual bool ShouldStartRemoteCall() override;
	virtual void StartDelayedAbility() override;
	TObjectPtr<class AOutlineManager> GetOutlineManager();

private:
	void DisableClairvoyance();

protected:
	// 투시능력이 몇초동안 이뤄질지 지정합니다.
	UPROPERTY(EditAnywhere)
	float BaseAbilityDuration;

	TWeakObjectPtr<AOutlineManager> OutlineManager;
	
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UMaterialInstanceDynamic> EffectMaterial;
	
	TWeakObjectPtr<UUserWidget> ClairvoyanceWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ClairvoyanceWidgetClass;
	
private:
	FTimerHandle ClairvoyanceTimer;
};
