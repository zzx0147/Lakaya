// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "GameFramework/Actor.h"
#include "CaptureArea.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureAreaStateOnChangedSignature, ECaptureAreaState);
DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureAreaTeamOnChangedSignature, ETeam);

UCLASS()
class LAKAYA_API ACaptureArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureArea();

	FORCEINLINE const ECaptureAreaState& GetCurrentCaptureAreaState() const { return CurrentCaptureAreaState; }
	FORCEINLINE void SetCurrentCaptureAreaState(const ECaptureAreaState& NewState) { CurrentCaptureAreaState = NewState; }

	FORCEINLINE const ETeam& GetCurrentCaptureAreaTeam() const { return CurrentCaptureAreaTeam; }
	FORCEINLINE void SetCurrentCaptureAreaTeam(const ETeam& NewTeam) { CurrentCaptureAreaTeam = NewTeam; }

	/**
	 * @brief Enum타입을 String으로 바꿔줍니다.
	 * @param EnumValue 타입을 String으로 바꿔줄 Enum입니다.
	 * @return Enum타입이 String타입으로 바뀌어서 리턴합니다.
	 */
	UFUNCTION()
	FString GetEnumAsString(const ECaptureAreaState& EnumValue);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * @brief 다른 액터와 충돌됐을 때, 실행되는 함수입니다.
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

	/**
	 * @brief 점령중인 플레이어 리스트에 플레이어를 넣어주는 함수입니다.
	 * @param PlayerTeam 리스트에 추가할 플레이어의 소속 팀입니다.
	 * @param Player 리스트에 추가할 플레이어입니다.
	 */
	UFUNCTION()
	void AddToOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player);

	UFUNCTION()
	void RemoveFromOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player);

	/**
	 * @brief 점령구역에 누군가가 나가거나 들어오게 되었을 때, 점령구역에 인원수를 체크하여, 점령상태를 업데이트 해주는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태를 나타냅니다.
	 */
	UFUNCTION()
	void UpdateCaptureAreaState(const ECaptureAreaState& CaptureAreaState);
	
	/**
	 * @brief 점령구역에 Anti팀의 인원이 1명이상 이며 Pro팀의 인원이 0명일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleAntiTeam(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원이 0명 이며 Pro팀의 인원이 1명이상일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleProTeam(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 1명이상 일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleOpposite(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 0명일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleNone(const ECaptureAreaState& CaptureAreaState);

	UFUNCTION()
	void UpdateCaptureProgress();

private:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(EditAnywhere)
	FVector CaptureAreaRange;
	
	UPROPERTY(Replicated)
	ECaptureAreaState CurrentCaptureAreaState = ECaptureAreaState::None;

	UPROPERTY(Replicated)
	ETeam CurrentCaptureAreaTeam = ETeam::None;
	
	TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> OccupyingPlayerList;
	
	FCaptureAreaStateOnChangedSignature CaptureAreaStateOnChangedSignature;
	FCaptureAreaTeamOnChangedSignature CaptureAreaTeamOnChangedSignature;

	UPROPERTY()
	float AntiTeamCaptureProgress;

	UPROPERTY()
	float ProTeamCaptureProgress;

	UPROPERTY()
	float CaptureSpeed;

	FTimerHandle CaptureProgressTimerHandle;
};