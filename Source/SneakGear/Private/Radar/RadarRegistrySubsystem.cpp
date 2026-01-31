#include "Radar/RadarRegistrySubsystem.h"

void URadarRegistrySubsystem::Compact()
{
	Actors.RemoveAll([](const TWeakObjectPtr<AActor>& Item)
	{
		return !Item.IsValid();
	});
}

void URadarRegistrySubsystem::RegisterActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	for (auto Item : Actors)
	{
		if (Item.Get() == Actor)
		{
			return;
		}
	}

	Actors.Add(Actor);
}

void URadarRegistrySubsystem::UnregisterActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actors.RemoveAll([Actor](const TWeakObjectPtr<AActor>& Item)
	{
		return !Item.IsValid() || Item.Get() == Actor;
	});
}
