#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "IndividualGameResultWidget.generated.h"

UCLASS()
class LAKAYA_API UIndividualGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetScore(const uint16& ScoreCount) const;
	void SetKill(const uint16& KillCount) const;
	void SetDeath(const uint16& DeathCount) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> First_Rank_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> Second_Rank_Image;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> Third_Rank_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> Fourth_Rank_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> Fifth_Rank_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> Sixth_Rank_Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Score_Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Kill_Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Death_Text;
};