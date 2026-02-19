#include "AI/GuardSpawner.h"

#include "AI/PatrolPath.h"
#include "Characters/AI/GuardCharacter.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

AGuardSpawner::AGuardSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	auto* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AGuardSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSpawnOnBeginPlay)
	{
		SpawnGuards();
	}
}

void AGuardSpawner::SpawnGuards()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GuardClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GuardSpawner '%s' has no GuardClass set."), *GetName());
		return;
	}

	CompactSpawnedGuards();

	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		const FTransform SpawnTransform = BuildSpawnTransform(Index);

		AGuardCharacter* Guard = GetWorld()->SpawnActorDeferred<AGuardCharacter>(
			GuardClass,
			SpawnTransform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (!Guard)
		{
			continue;
		}

		Guard->SetPatrolPath(PatrolPath);
		Guard->FinishSpawning(SpawnTransform);
		SpawnedGuards.Add(Guard);
	}
}

void AGuardSpawner::DespawnSpawnedGuards()
{
	if (!HasAuthority())
	{
		return;
	}

	for (auto Guard : SpawnedGuards)
	{
		if (IsValid(Guard))
		{
			Guard->Destroy();
		}
	}

	SpawnedGuards.Reset();
}

int32 AGuardSpawner::GetSpawnedGuardCount() const
{
	int32 Count = 0;
	for (const auto Guard : SpawnedGuards)
	{
		if (IsValid(Guard))
		{
			Count++;
		}
	}

	return Count;
}

void AGuardSpawner::CompactSpawnedGuards()
{
	SpawnedGuards.RemoveAll([](const TObjectPtr<AGuardCharacter>& Guard)
	{
		return !IsValid(Guard);
	});
}

FTransform AGuardSpawner::BuildSpawnTransform(int32 Index) const
{
	const FVector BaseLocation = GetActorLocation();
	const FRotator BaseRotation = GetActorRotation();

	if (SpawnCount <= 1 || SpawnRadius <= KINDA_SMALL_NUMBER)
	{
		return FTransform(BaseRotation, BaseLocation);
	}

	const float Angle = (2.f * PI * static_cast<float>(Index)) / static_cast<float>(SpawnCount);
	const FVector LocalOffset(FMath::Cos(Angle) * SpawnRadius, FMath::Sin(Angle) * SpawnRadius, 0.f);
	const FVector WorldOffset = BaseRotation.RotateVector(LocalOffset);

	return FTransform(BaseRotation, BaseLocation + WorldOffset);
}
