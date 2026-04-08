#include "UI/EventLogSubsystem.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void UEventLogSubsystem::AddEvent(const FText& Message, EGameEventCategory Category)
{
	if (Message.IsEmpty())
	{
		return;
	}

	FGameEventEntry Entry;
	Entry.EventId = NextEventId++;
	Entry.WorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	Entry.Category = Category;
	Entry.Message = Message;

	Events.Add(Entry);
	OnEventAdded.Broadcast(Entry);

	if (MaxStoredEvents > 0 && Events.Num() > MaxStoredEvents)
	{
		const int32 Overflow = Events.Num() - MaxStoredEvents;
		Events.RemoveAt(0, Overflow);
	}
}

void UEventLogSubsystem::ReportDamageTaken(AActor* Victim, float DamageAmount, AActor* DamageCauser)
{
	const FText VictimName = FText::FromString(GetNameSafe(Victim));
	const FText CauserName = FText::FromString(GetNameSafe(DamageCauser));
	const APawn* VictimPawn = Cast<APawn>(Victim);
	const AController* VictimController = VictimPawn ? VictimPawn->GetController() : nullptr;
	const bool bVictimIsPlayer = VictimPawn
		? (VictimPawn->IsPlayerControlled() || (VictimController && VictimController->IsA<APlayerController>()))
		: false;

	if (DamageCauser)
	{
		const FText FormatText = bVictimIsPlayer
			                         ? NSLOCTEXT("SneakGear", "EventPlayerDamageWithCauser", "Player took {0} damage from {1}")
			                         : NSLOCTEXT("SneakGear", "EventGuardDamageWithCauser", "Guard {0} took {1} damage from {2}");
		AddEvent(bVictimIsPlayer
			         ? FText::Format(FormatText, FText::AsNumber(FMath::RoundToInt(DamageAmount)), CauserName)
			         : FText::Format(FormatText, VictimName, FText::AsNumber(FMath::RoundToInt(DamageAmount)), CauserName),
		         EGameEventCategory::Damage);
		return;
	}

	const FText FormatText = bVictimIsPlayer ? NSLOCTEXT("SneakGear", "EventPlayerDamage", "Player took {0} damage")
	                                         : NSLOCTEXT("SneakGear", "EventGuardDamage", "Guard {0} took {1} damage");

	AddEvent(bVictimIsPlayer
		         ? FText::Format(FormatText, FText::AsNumber(FMath::RoundToInt(DamageAmount)))
		         : FText::Format(FormatText, VictimName, FText::AsNumber(FMath::RoundToInt(DamageAmount))),
	         EGameEventCategory::Damage);
}

void UEventLogSubsystem::ReportItemPickedUp(AActor* Picker, const FText& ItemName)
{
	const FText PickerName = FText::FromString(GetNameSafe(Picker));
	AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventPickup", "{0} picked up {1}"), PickerName, ItemName),
	         EGameEventCategory::Pickup);
}

void UEventLogSubsystem::ReportItemUsed(AActor* User, const FText& ItemName, const FText& SlotLabel)
{
	const FText UserName = FText::FromString(GetNameSafe(User));
	AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventItemUsed", "{0} used {1} ({2})"), UserName, ItemName, SlotLabel),
	         EGameEventCategory::Info);
}

void UEventLogSubsystem::ReportItemEquipped(AActor* User, const FText& ItemName)
{
	const FText UserName = FText::FromString(GetNameSafe(User));
	AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventItemEquipped", "{0} equipped {1}"), UserName, ItemName),
	         EGameEventCategory::Info);
}

void UEventLogSubsystem::ReportGuardAwarenessChanged(AActor* GuardActor, const FText& NewState)
{
	const FText GuardName = FText::FromString(GetNameSafe(GuardActor));
	AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventAwareness", "{0} awareness is now {1}"), GuardName, NewState),
	         EGameEventCategory::Awareness);
}

void UEventLogSubsystem::ReportGuardStartedFiring(AActor* GuardActor, AActor* TargetActor)
{
	const FText GuardName = FText::FromString(GetNameSafe(GuardActor));
	const FText TargetName = FText::FromString(GetNameSafe(TargetActor));

	if (TargetActor)
	{
		AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventGuardStartedFiringAtTarget", "{0} started firing at {1}"),
				GuardName, TargetName),
			EGameEventCategory::Awareness);
		return;
	}

	AddEvent(FText::Format(NSLOCTEXT("SneakGear", "EventGuardStartedFiring", "{0} started firing"), GuardName),
		EGameEventCategory::Awareness);
}

void UEventLogSubsystem::GetEventsAfter(int32 LastSeenEventId, TArray<FGameEventEntry>& OutEvents) const
{
	OutEvents.Reset();

	for (const FGameEventEntry& Entry : Events)
	{
		if (Entry.EventId > LastSeenEventId)
		{
			OutEvents.Add(Entry);
		}
	}
}
