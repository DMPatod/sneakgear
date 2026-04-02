#include "Game/SandboxGameState.h"

#include "UI/EventLogSubsystem.h"

void ASandboxGameState::StartMission(const FText& InMissionName, const FText& InObjectiveText)
{
	MissionName = InMissionName;
	MissionObjectiveText = InObjectiveText;
	MissionCompletionText = FText::GetEmpty();
	MissionStartWorldSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	MissionElapsedSeconds = 0.f;
	MissionState = EMissionState::Active;
	BroadcastMissionStateChanged();

	if (UEventLogSubsystem* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		if (!MissionName.IsEmpty())
		{
			EventLog->AddEvent(FText::Format(
				NSLOCTEXT("SneakGear", "MissionStarted", "Mission started: {0}"),
				MissionName));
		}

		if (!MissionObjectiveText.IsEmpty())
		{
			EventLog->AddEvent(FText::Format(
				NSLOCTEXT("SneakGear", "MissionObjective", "Objective: {0}"),
				MissionObjectiveText));
		}
	}
}

void ASandboxGameState::CompleteMission(const FText& InCompletionText)
{
	const float CurrentWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : MissionStartWorldSeconds;
	MissionElapsedSeconds = FMath::Max(0.f, CurrentWorldTime - MissionStartWorldSeconds);

	const FText BaseCompletionText = !InCompletionText.IsEmpty()
		? InCompletionText
		: NSLOCTEXT("SneakGear", "MissionCompletedDefault", "Mission complete");
	const FText ElapsedText = BuildMissionElapsedText(MissionElapsedSeconds);

	MissionState = EMissionState::Completed;
	MissionCompletionText = FText::Format(
		NSLOCTEXT("SneakGear", "MissionCompletedWithTime", "{0} - Time spent: {1}"),
		BaseCompletionText,
		ElapsedText);
	BroadcastMissionStateChanged();

	if (UEventLogSubsystem* EventLog = GetWorld() ? GetWorld()->GetSubsystem<UEventLogSubsystem>() : nullptr)
	{
		EventLog->AddEvent(MissionCompletionText);
	}
}

void ASandboxGameState::ResetMission()
{
	MissionState = EMissionState::Inactive;
	MissionName = FText::GetEmpty();
	MissionObjectiveText = FText::GetEmpty();
	MissionCompletionText = FText::GetEmpty();
	MissionStartWorldSeconds = 0.f;
	MissionElapsedSeconds = 0.f;
	BroadcastMissionStateChanged();
}

bool ASandboxGameState::IsMissionActive() const
{
	return MissionState == EMissionState::Active;
}

bool ASandboxGameState::IsMissionCompleted() const
{
	return MissionState == EMissionState::Completed;
}

EMissionState ASandboxGameState::GetMissionState() const
{
	return MissionState;
}

FText ASandboxGameState::GetMissionName() const
{
	return MissionName;
}

FText ASandboxGameState::GetMissionObjectiveText() const
{
	return MissionObjectiveText;
}

FText ASandboxGameState::GetMissionCompletionText() const
{
	return MissionCompletionText;
}

float ASandboxGameState::GetMissionElapsedSeconds() const
{
	if (MissionState == EMissionState::Active && GetWorld())
	{
		return FMath::Max(0.f, GetWorld()->GetTimeSeconds() - MissionStartWorldSeconds);
	}

	return MissionElapsedSeconds;
}

FText ASandboxGameState::GetMissionElapsedText() const
{
	return BuildMissionElapsedText(GetMissionElapsedSeconds());
}

void ASandboxGameState::BroadcastMissionStateChanged()
{
	OnMissionStateChanged.Broadcast(MissionState);
}

FText ASandboxGameState::BuildMissionElapsedText(float ElapsedSeconds) const
{
	const int32 TotalSeconds = FMath::Max(0, FMath::RoundToInt(ElapsedSeconds));
	const int32 Hours = TotalSeconds / 3600;
	const int32 Minutes = (TotalSeconds % 3600) / 60;
	const int32 Seconds = TotalSeconds % 60;

	const FString Formatted = Hours > 0
		? FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds)
		: FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	return FText::FromString(Formatted);
}
