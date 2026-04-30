#include "Radar/RadarRegistrySubsystem.h"

#include "Guards/Components/GuardAwarenessComponent.h"

namespace
{
FVector2D WorldToRadarPosition(const FVector& ViewerLocation, const float ReferenceYawDeg, const FVector& TargetLocation,
                               const float RadarRadiusPx, const float RadarRangeWorld)
{
	const FVector Delta = TargetLocation - ViewerLocation;
	const FVector2D Delta2D(Delta.X, Delta.Y);

	const float YawRad = FMath::DegreesToRadians(ReferenceYawDeg);
	const float CosYaw = FMath::Cos(-YawRad);
	const float SinYaw = FMath::Sin(-YawRad);
	const FVector2D Rotated(Delta2D.X * CosYaw - Delta2D.Y * SinYaw, Delta2D.X * SinYaw + Delta2D.Y * CosYaw);

	const float Distance = Rotated.Size();
	const float ClampedDistance = FMath::Min(Distance, RadarRangeWorld);
	const FVector2D Direction = Distance > 1.f ? Rotated / Distance : FVector2D::ZeroVector;
	return Direction * (ClampedDistance / RadarRangeWorld * RadarRadiusPx);
}
}

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

	if (UGuardAwarenessComponent* Awareness = Actor->FindComponentByClass<UGuardAwarenessComponent>())
	{
		Awareness->OnAwarenessStateChanged.AddUObject(this, &URadarRegistrySubsystem::HandleGuardAwarenessChanged);
	}


	OnContactsDirty.Broadcast();
}

void URadarRegistrySubsystem::UnregisterActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (UGuardAwarenessComponent* Awareness = Actor->FindComponentByClass<UGuardAwarenessComponent>())
	{
		Awareness->OnAwarenessStateChanged.RemoveAll(this);
	}

	Actors.RemoveAll([Actor](const TWeakObjectPtr<AActor>& Item)
	{
		return !Item.IsValid() || Item.Get() == Actor;
	});

	OnContactsDirty.Broadcast();
}

void URadarRegistrySubsystem::HandleGuardAwarenessChanged()
{
	OnContactsDirty.Broadcast();
}

void URadarRegistrySubsystem::BuildGuardContacts(const APawn* ViewerPawn, float ReferenceYawDeg, float RadarRadiusPx,
                                                 float RadarRangeWorld, TArray<FRadarContact>& OutContacts)
{
	OutContacts.Reset();

	if (!ViewerPawn || RadarRangeWorld <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	Compact();

	const FVector ViewerLocation = ViewerPawn->GetActorLocation();
	OutContacts.Reserve(Actors.Num());

	for (const TWeakObjectPtr<AActor>& WeakActor : Actors)
	{
		const AActor* Actor = WeakActor.Get();
		if (!Actor)
		{
			continue;
		}

		const UGuardAwarenessComponent* Awareness = Actor->FindComponentByClass<UGuardAwarenessComponent>();
		if (!Awareness)
		{
			continue;
		}

		FRadarContact Contact;
		Contact.Awareness = Awareness->GetAwareness();
		Contact.VisionRange = Awareness->GetVisionRange();
		Contact.HearingRange = Awareness->GetHearingRange();
		Contact.bHasLOS = Awareness->HasLineOfSight();
		Contact.LookYawOnRadarDeg = FMath::FindDeltaAngleDegrees(ReferenceYawDeg, Actor->GetActorRotation().Yaw);
		Contact.RadarPos = WorldToRadarPosition(ViewerLocation, ReferenceYawDeg, Actor->GetActorLocation(), RadarRadiusPx, RadarRangeWorld);
		OutContacts.Add(Contact);
	}
}
