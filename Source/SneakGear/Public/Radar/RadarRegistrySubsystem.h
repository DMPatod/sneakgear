// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InterchangeTranslatorBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "RadarRegistrySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SNEAKGEAR_API URadarRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Compact();

	void RegisterActor(AActor* Actor);
	void UnregisterActor(AActor* Actor);

	const TArray<TWeakObjectPtr<AActor>>& GetActors() const
	{
		return Actors;
	}

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Actors;
};
