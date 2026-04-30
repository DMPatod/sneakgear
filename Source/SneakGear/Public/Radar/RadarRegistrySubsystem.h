#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/RadarTypes.h"
#include "RadarRegistrySubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRadarContactsDirty);

UCLASS()
class SNEAKGEAR_API URadarRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Compact();

	void RegisterActor(AActor* Actor);
	void UnregisterActor(AActor* Actor);
	void BuildGuardContacts(const APawn* ViewerPawn, float ReferenceYawDeg, float RadarRadiusPx, float RadarRangeWorld,
	                       TArray<FRadarContact>& OutContacts);

	const TArray<TWeakObjectPtr<AActor>>& GetActors() const
	{
		return Actors;
	}

	FOnRadarContactsDirty OnContactsDirty;

private:
	void HandleGuardAwarenessChanged();

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Actors;
};
