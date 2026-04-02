#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SandboxGameState.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Inactive,
	Active,
	Completed
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMissionStateChanged, EMissionState);

UCLASS()
class SNEAKGEAR_API ASandboxGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Mission")
	void StartMission(const FText& InMissionName, const FText& InObjectiveText);

	UFUNCTION(BlueprintCallable, Category="Mission")
	void CompleteMission(const FText& InCompletionText = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category="Mission")
	void ResetMission();

	UFUNCTION(BlueprintPure, Category="Mission")
	bool IsMissionActive() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	bool IsMissionCompleted() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	EMissionState GetMissionState() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	FText GetMissionName() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	FText GetMissionObjectiveText() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	FText GetMissionCompletionText() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	float GetMissionElapsedSeconds() const;

	UFUNCTION(BlueprintPure, Category="Mission")
	FText GetMissionElapsedText() const;

	FOnMissionStateChanged& OnMissionStateChangedEvent()
	{
		return OnMissionStateChanged;
	}

private:
	void BroadcastMissionStateChanged();
	FText BuildMissionElapsedText(float ElapsedSeconds) const;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	EMissionState MissionState = EMissionState::Inactive;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	FText MissionName;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	FText MissionObjectiveText;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	FText MissionCompletionText;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	float MissionStartWorldSeconds = 0.f;

	UPROPERTY(VisibleAnywhere, Category="Mission")
	float MissionElapsedSeconds = 0.f;

	FOnMissionStateChanged OnMissionStateChanged;
};
