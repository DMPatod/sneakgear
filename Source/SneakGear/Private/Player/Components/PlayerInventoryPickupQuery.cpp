#include "Player/Components/PlayerInventoryPickupQuery.h"

#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Player/Components/PlayerInventoryComponent.h"

AActor* FPlayerInventoryPickupQuery::FindBestNearbyFloorPickup(const UPlayerInventoryComponent& Inventory, float SearchRadius)
{
	UWorld* World = Inventory.GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const AActor* OwnerActor = Inventory.GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FloorItemPickup), false, OwnerActor);
	const FVector Center = OwnerActor->GetActorLocation();
	const FCollisionShape Shape = FCollisionShape::MakeSphere(FMath::Max(SearchRadius, 1.f));

	const bool bHasOverlaps = World->OverlapMultiByObjectType(
		Overlaps,
		Center,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		Shape,
		QueryParams
	);

	if (!bHasOverlaps)
	{
		return nullptr;
	}

	AActor* BestPickup = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* PickupActor = Result.GetActor();
		if (!PickupActor)
		{
			continue;
		}

		const UPlayerItemPickupComponent* PickupComponent = PickupActor->FindComponentByClass<UPlayerItemPickupComponent>();
		if (!PickupComponent || !PickupComponent->GetItemDefinition())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(Center, PickupActor->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestPickup = PickupActor;
		}
	}

	return BestPickup;
}

bool FPlayerInventoryPickupQuery::PickupRequiresWeaponSwap(const UPlayerInventoryComponent& Inventory,
                                                           const UPlayerItemPickupComponent* PickupComponent)
{
	if (!PickupComponent)
	{
		return false;
	}

	const FPlayerInventoryItem PickupItem = PickupComponent->GetPickupItem();
	if (PickupItem.SlotType != EPlayerItemSlot::PrimaryWeapon && PickupItem.SlotType != EPlayerItemSlot::SecondaryWeapon)
	{
		return false;
	}

	const FPlayerInventoryItem* ExistingItem = Inventory.ResolveSlot(PickupItem.SlotType);
	return ExistingItem && ExistingItem->IsValid();
}
