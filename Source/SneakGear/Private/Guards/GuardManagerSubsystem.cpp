#include "Guards/GuardManagerSubsystem.h"

#include "Guards/GuardCharacter.h"

void UGuardManagerSubsystem::Compact()
{
	Guards.RemoveAll([](const TWeakObjectPtr<AGuardCharacter>& Item)
	{
		return !Item.IsValid();
	});
}

void UGuardManagerSubsystem::RegisterGuard(AGuardCharacter* Guard)
{
	if (!Guard)
	{
		return;
	}

	for (const auto& Item : Guards)
	{
		if (Item.Get() == Guard)
		{
			return;
		}
	}

	Guards.Add(Guard);
}

void UGuardManagerSubsystem::UnregisterGuard(AGuardCharacter* Guard)
{
	if (!Guard)
	{
		return;
	}

	Guards.RemoveAll([Guard](const TWeakObjectPtr<AGuardCharacter>& Item)
	{
		return !Item.IsValid() || Item.Get() == Guard;
	});
}

int32 UGuardManagerSubsystem::GetGuardCount() const
{
	int32 Count = 0;
	for (const auto& Item : Guards)
	{
		if (Item.IsValid())
		{
			Count++;
		}
	}

	return Count;
}

void UGuardManagerSubsystem::SetGlobalTargetActor(AActor* NewTarget)
{
	Compact();

	for (const auto& Item : Guards)
	{
		if (auto* Guard = Item.Get())
		{
			Guard->SetTargetActor(NewTarget);
		}
	}
}

void UGuardManagerSubsystem::AddAwarenessToAll(float DeltaAwareness)
{
	Compact();

	for (const auto& Item : Guards)
	{
		if (auto* Guard = Item.Get())
		{
			Guard->AddAwareness(DeltaAwareness);
		}
	}
}

TArray<AGuardCharacter*> UGuardManagerSubsystem::GetAliveGuards() const
{
	TArray<AGuardCharacter*> Result;
	Result.Reserve(Guards.Num());

	for (const auto& Item : Guards)
	{
		if (auto* Guard = Item.Get())
		{
			Result.Add(Guard);
		}
	}

	return Result;
}
