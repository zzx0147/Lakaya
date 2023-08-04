// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerTeam.h"
#include "GameFramework/Actor.h"
#include "CaptureArena.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOccupyingPlayerListOnChangedSignature, const TMap<EPlayerTeam, TSoftObjectPtr<ALakayaBasePlayerState>>>, OccupyingPlayerList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureArenaStateOnChangedSignature, ECaptureArenaState, CurrentCaptureArenaState);

UENUM()
enum class ECaptureArenaState : uint8
{
	// 아무도 점령하지 않은 상태입니다.
	None UMETA(DisplayerName = "None"),

	// Anti팀에서 점령을 시도하는 상태입니다.
	AntiProgress UMETA(DisplayerName = "AntiProgress"),

	// Anti팀이 점령한 상태입니다.
	Anti UMETA(DisplayerName = "Anti"),

	// Pro팀에서 점령을 시도하는 상태입니다.
	ProProgress UMETA(DisplayerName = "ProProgress"),

	// Pro팀에서 점령한 상태입니다.
	Pro UMETA(DisplayerName = "Pro"),

	// Anti님과 Pro팀이 대치하고 있는 상태입니다.
	Opposite UMETA(DisPlayerName = "Opposite"),
};

UCLASS()
class LAKAYA_API ACaptureArena : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureArena();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * @brief 
	 * @param OverlappedComp 겹친 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherActor 겹친 다른 액터를 나타내는 포인터입니다.
	 * @param OtherComp 겹친 다른 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherBodyIndex 겹친 다른 컴포넌트와의 관계를 설명하는 정수 인덱스입니다.
	 * @param bFromSweep 겹침 여부를 나타내는 bool값입니다.
	 * @param SweepResult 겹친 결과를 반환하는 FHitResult 구조체로부터 상수입니다.
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void AddToOccupyPlayerList(EPlayerTeam Team, ALakayaBasePlayerState* Player);

	UFUNCTION()
	void RemoveFromOccupyPlayerList(EPlayerTeam Team, ALakayaBasePlayerState* Player);
	
	UFUNCTION()
	void CheckCaptureArenaInPlayer();
	
	UFUNCTION()
	void OnRep_BroadCastCaptureState();
	
private:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(ReplicatedUsing = OnRep_BroadCastCaptureState)
	ECaptureArenaState CurrentCaptureArenaState = ECaptureArenaState::None;
	
	// UPROPERTY(Replicated)
	TMap<EPlayerTeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> OccupyingPlayerList;

	// FOccupyingPlayerListOnChangedSignature OccupyingPlayerListOnChangedSignature;
	FCaptureArenaStateOnChangedSignature CaptureArenaStateOnChangedSignature;
};