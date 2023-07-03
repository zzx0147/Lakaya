#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/ArmedCharacter.h"
#include "SkillWidget.generated.h"

USTRUCT(BlueprintType)
struct FSkillWidgetTextureStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureBackgroundImage;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureBackgroundImage;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* RButtonSkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* RButtonSkillTextureBackgroundImage;
};


UCLASS()
class LAKAYA_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetCharacter(const FName& CharacterName);
	class USkillProgressBar* GetSkillProgressBar(EAbilityKind AbilityKind);
	
	
private:
	TWeakObjectPtr<class USkillProgressBar> QSkillProgressBar;			// 레나 Q 스킬
	TWeakObjectPtr<USkillProgressBar> ESkillProgressBar;			// 레나 E 스킬
	TWeakObjectPtr<USkillProgressBar> RButtonSkillProgressBar;	// 레나 우클릭 스킬

	UPROPERTY(EditAnywhere)
	TMap<FName, FSkillWidgetTextureStruct> SkillWidgetTextureMap;
};