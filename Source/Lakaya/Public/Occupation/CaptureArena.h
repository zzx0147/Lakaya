// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "GameFramework/Actor.h"
#include "CaptureArena.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureArenaStateOnChangedSignature, ECaptureArenaState);
DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureArenaTeamOnChangedSignature, ETeam);

UCLASS()
class LAKAYA_API ACaptureArena : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureArena();

	FORCEINLINE const ECaptureArenaState& GetCurrentCaptureArenaState() const { return CurrentCaptureArenaState; }
	FORCEINLINE void SetCurrentCaptureArenaState(const ECaptureArenaState& NewState) { CurrentCaptureArenaState = NewState; }

	FORCEINLINE const ETeam& GetCurrentCaptureArenaTeam() const { return CurrentCaptureArenaTeam; }
	FORCEINLINE void SetCurrentCaptureArenaTeam(const ETeam& NewTeam) { CurrentCaptureArenaTeam = NewTeam; }
	/**
	 * @brief Enum타입을 String으로 바꿔줍니다.
	 * @param EnumValue 타입을 String으로 바꿔줄 Enum입니다.
	 * @return Enum타입이 String타입으로 바뀌어서 리턴합니다.
	 */
	UFUNCTION()
	FString GetEnumAsString(const ECaptureArenaState& EnumValue);
	
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
	 * @brief 점령구역에 누군가가 나가거나 들어오게 되었을 때 실행되며, 점령구역에 인원수를 점검하여, 점령상태를 바꾸는 함수입니다.
	 * @param PlayerTeam 체크할 대상의 팀입니다.
	*/
	UFUNCTION()
	void CheckCaptureArenaInPlayer(const ETeam& PlayerTeam);

	/**
	 * @brief 점령구역 상태가 CheckCaptureArenaInPlayer함수를 통해 인원수를 체크하여 상태를 업데이트해주는 함수입니다.
	 * @param AntiTeamPlayerCount 현재 점령구역에 있는 Anti팀의 플레이어 수 입니다.
	 * @param ProTeamPlayerCount 현재 점령구역에 있는 Pro팀의 플레이어 수 입니다.
	 * @param CaptureArenaState 현재 점령구역의 상태를 나타냅니다.
	 */
	UFUNCTION()
	void UpdateCaptureArenaState(const uint8& AntiTeamPlayerCount, const uint8& ProTeamPlayerCount, const ECaptureArenaState& CaptureArenaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원이 1명이상 이며 Pro팀의 인원이 0명일 때 호출되는 함수입니다.
	 * @param CaptureArenaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureArenaHandleAntiTeam(const ECaptureArenaState& CaptureArenaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원이 0명 이며 Pro팀의 인원이 1명이상일 때 호출되는 함수입니다.
	 * @param CaptureArenaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureArenaHandleProTeam(const ECaptureArenaState& CaptureArenaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 1명이상 일 때 호출되는 함수입니다.
	 * @param CaptureArenaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureArenaHandleOpposite(const ECaptureArenaState& CaptureArenaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 0명일 때 호출되는 함수입니다.
	 * @param CaptureArenaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureArenaHandleNone(const ECaptureArenaState& CaptureArenaState);

	UFUNCTION()
	void UpdateCaptureProgress();

	
private:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(Replicated)
	ECaptureArenaState CurrentCaptureArenaState = ECaptureArenaState::None;

	UPROPERTY(Replicated)
	ETeam CurrentCaptureArenaTeam = ETeam::None;
	
	TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> OccupyingPlayerList;
	
	FCaptureArenaStateOnChangedSignature CaptureArenaStateOnChangedSignature;
	FCaptureArenaTeamOnChangedSignature CaptureArenaTeamOnChangedSignature;

	UPROPERTY()
	float AntiTeamCaptureProgress;

	UPROPERTY()
	float ProTeamCaptureProgress;

	UPROPERTY()
	float CaptureSpeed;

	FTimerHandle CaptureProgressTimerHandle;
};