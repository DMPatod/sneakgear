#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EventLogSubsystem.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EGameEventCategory : uint8
{
	Info,
	Damage,
	Pickup,
	Awareness
};

USTRUCT(BlueprintType)
struct FGameEventEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Event Log")
	int32 EventId = 0;

	UPROPERTY(BlueprintReadOnly, Category="Event Log")
	float WorldTimeSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Event Log")
	EGameEventCategory Category = EGameEventCategory::Info;

	UPROPERTY(BlueprintReadOnly, Category="Event Log")
	FText Message;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameEventAdded, const FGameEventEntry&);

UCLASS()
class SNEAKGEARUI_API UEventLogSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Event Log")
	void AddEvent(const FText& Message, EGameEventCategory Category = EGameEventCategory::Info);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportDamageTaken(AActor* Victim, float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportItemPickedUp(AActor* Picker, const FText& ItemName);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportItemUsed(AActor* User, const FText& ItemName, const FText& SlotLabel);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportItemEquipped(AActor* User, const FText& ItemName);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportGuardAwarenessChanged(AActor* GuardActor, const FText& NewState);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void ReportGuardStartedFiring(AActor* GuardActor, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="Event Log")
	void GetEventsAfter(int32 LastSeenEventId, TArray<FGameEventEntry>& OutEvents) const;

	FOnGameEventAdded& OnEventAddedEvent()
	{
		return OnEventAdded;
	}

private:
	UPROPERTY()
	TArray<FGameEventEntry> Events;

	int32 NextEventId = 1;

	UPROPERTY(EditAnywhere, Category="Event Log")
	int32 MaxStoredEvents = 128;

	FOnGameEventAdded OnEventAdded;
};
