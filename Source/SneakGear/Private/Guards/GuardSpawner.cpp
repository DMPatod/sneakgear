#include "Guards/GuardSpawner.h"

#include "Guards/Patrol/PatrolPath.h"
#include "Guards/GuardCharacter.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Misc/DataValidation.h"

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
	const int32 ExistingGuardCount = SpawnedGuards.Num();
	if (ExistingGuardCount >= SpawnCount)
	{
		return;
	}

	for (int32 Index = ExistingGuardCount; Index < SpawnCount; ++Index)
	{
		APatrolPath* AssignedPatrolPath = ResolvePatrolPathForGuard(Index);
		const FTransform SpawnTransform = BuildSpawnTransform(Index, AssignedPatrolPath);

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

		Guard->SetPatrolPath(AssignedPatrolPath);
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

EDataValidationResult AGuardSpawner::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!GuardClass)
	{
		Context.AddError(FText::FromString(TEXT("GuardClass must be assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (SpawnCount <= 0)
	{
		Context.AddError(FText::FromString(TEXT("SpawnCount must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	for (int32 Index = 0; Index < PatrolPaths.Num(); ++Index)
	{
		if (!IsValid(PatrolPaths[Index]))
		{
			Context.AddWarning(FText::FromString(FString::Printf(TEXT("PatrolPaths[%d] is null and will be skipped."), Index)));
			if (Result == EDataValidationResult::NotValidated)
			{
				Result = EDataValidationResult::Valid;
			}
		}
	}

	return Result;
}

void AGuardSpawner::CompactSpawnedGuards()
{
	SpawnedGuards.RemoveAll([](const TObjectPtr<AGuardCharacter>& Guard)
	{
		return !IsValid(Guard);
	});
}

FTransform AGuardSpawner::BuildSpawnTransform(int32 Index, const APatrolPath* AssignedPatrolPath) const
{
	if (IsValid(AssignedPatrolPath) && AssignedPatrolPath->Num() > 0)
	{
		return FTransform(AssignedPatrolPath->GetActorRotation(), AssignedPatrolPath->GetWorldPoint(0));
	}

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

APatrolPath* AGuardSpawner::ResolvePatrolPathForGuard(int32 SpawnIndex) const
{
	if (PatrolPaths.IsEmpty())
	{
		return nullptr;
	}

	// Direct mapping: guard i uses PatrolPaths[i] when available.
	if (PatrolPaths.IsValidIndex(SpawnIndex) && IsValid(PatrolPaths[SpawnIndex]))
	{
		return PatrolPaths[SpawnIndex];
	}

	// Fallback: use the first valid path if array is shorter or has gaps.
	for (APatrolPath* Path : PatrolPaths)
	{
		if (IsValid(Path))
		{
			return Path;
		}
	}

	return nullptr;
}
