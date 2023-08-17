#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/Team.h"
#include "RecordRow.generated.h"

UCLASS()
class LAKAYA_API URecordRow : public UUserWidget
{
	GENERATED_BODY()

public:
	URecordRow(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void SetRecordRowData(const ETeam& InWinTeam, const FText& InStartTimeText, const FText& InDurationText);
	void SetParentWidget(class URecordWidget* NewParent) { Parent = NewParent; }

private:
	void UpdateTextBlock();

private:
	TObjectPtr<class UTextBlock> WinTextBlock;
	FText WinText;
	TObjectPtr<UTextBlock> StartTimeTextBlock;
	FText StartTimeText;
	TObjectPtr<UTextBlock> DurationTextBlock;
	FText DurationText;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<URecordWidget> Parent;
};
