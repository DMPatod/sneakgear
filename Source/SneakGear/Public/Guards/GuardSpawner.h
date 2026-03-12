#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuardSpawner.generated.h"

class AGuardCharacter;
class APatrolPath;

UCLASS()
class SNEAKGEAR_API AGuardSpawner : public AActor
{
	GENERATED_BODY()

public:
	AGuardSpawner();

	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SpawnGuards();

	UFUNCTION(BlueprintCallable, Category="Spawn")
	void DespawnSpawnedGuards();

	UFUNCTION(BlueprintCallable, Category="Spawn")
	int32 GetSpawnedGuardCount() const;

protected:
	virtual void BeginPlay() override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	UPROPERTY(EditInstanceOnly, Category="Spawn")
	bool bAutoSpawnOnBeginPlay = true;

	UPROPERTY(EditInstanceOnly, Category="Spawn")
	TSubclassOf<AGuardCharacter> GuardClass;

	UPROPERTY(EditInstanceOnly, Category="Spawn", meta=(ClampMin="1"))
	int32 SpawnCount = 3;

	UPROPERTY(EditInstanceOnly, Category="Spawn", meta=(ClampMin="0.0"))
	float SpawnRadius = 250.f;

	UPROPERTY(EditInstanceOnly, Category="Patrol")
	TArray<TObjectPtr<APatrolPath>> PatrolPaths;

private:
	UPROPERTY()
	TArray<TObjectPtr<AGuardCharacter>> SpawnedGuards;

	void CompactSpawnedGuards();
	FTransform BuildSpawnTransform(int32 Index, const APatrolPath* AssignedPatrolPath) const;
	APatrolPath* ResolvePatrolPathForGuard(int32 SpawnIndex) const;
};
