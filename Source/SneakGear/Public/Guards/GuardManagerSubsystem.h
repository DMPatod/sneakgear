#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GuardManagerSubsystem.generated.h"

class AGuardCharacter;
class AActor;

UCLASS()
class SNEAKGEAR_API UGuardManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Compact();

	void RegisterGuard(AGuardCharacter* Guard);
	void UnregisterGuard(AGuardCharacter* Guard);

	UFUNCTION(BlueprintCallable, Category="Guard Manager")
	int32 GetGuardCount() const;

	UFUNCTION(BlueprintCallable, Category="Guard Manager")
	void SetGlobalTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category="Guard Manager")
	void AddAwarenessToAll(float DeltaAwareness);

	UFUNCTION(BlueprintCallable, Category="Guard Manager")
	TArray<AGuardCharacter*> GetAliveGuards() const;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AGuardCharacter>> Guards;
};
