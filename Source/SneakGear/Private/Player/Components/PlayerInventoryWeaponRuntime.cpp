#include "Player/Components/PlayerInventoryWeaponRuntime.h"

#include "GameFramework/Character.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "Weapon/WeaponBase.h"

void FPlayerInventoryWeaponRuntime::Initialize(UPlayerInventoryComponent& Inventory)
{
	Inventory.PrimaryWeaponRuntime.WeaponActor = SpawnWeapon(Inventory, Inventory.PrimaryWeaponClass);
	Inventory.SecondaryWeaponRuntime.WeaponActor = SpawnWeapon(Inventory, Inventory.SecondaryWeaponClass);
	Inventory.UnarmedWeapon = SpawnWeapon(Inventory, Inventory.UnarmedWeaponClass);
	Inventory.PrimaryWeaponRuntime.InClip = Inventory.PrimaryWeaponRuntime.WeaponActor
		? FMath::Max(Inventory.PrimaryWeaponRuntime.WeaponActor->GetClipSize(), 0)
		: -1;
	Inventory.SecondaryWeaponRuntime.InClip = Inventory.SecondaryWeaponRuntime.WeaponActor
		? FMath::Max(Inventory.SecondaryWeaponRuntime.WeaponActor->GetClipSize(), 0)
		: -1;

	if (Inventory.PrimaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, EPlayerItemSlot::PrimaryWeapon, Inventory.PrimaryWeaponRuntime.WeaponActor);
	}

	if (Inventory.SecondaryWeaponRuntime.WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, EPlayerItemSlot::SecondaryWeapon, Inventory.SecondaryWeaponRuntime.WeaponActor);
	}

	SyncAttachments(Inventory);
}

void FPlayerInventoryWeaponRuntime::Shutdown(UPlayerInventoryComponent& Inventory)
{
	if (Inventory.PrimaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(Inventory, Inventory.PrimaryWeaponRuntime);
	}

	if (Inventory.SecondaryWeaponRuntime.WeaponActor)
	{
		ClearRuntimeWeapon(Inventory, Inventory.SecondaryWeaponRuntime);
	}

	if (Inventory.UnarmedWeapon)
	{
		Inventory.UnarmedWeapon->Destroy();
		Inventory.UnarmedWeapon = nullptr;
	}
}

bool FPlayerInventoryWeaponRuntime::SetWeaponClassForSlot(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot,
                                                          TSubclassOf<AWeaponBase> WeaponClass)
{
	FPlayerInventoryWeaponSlotRuntime* Runtime = Inventory.ResolveWeaponRuntimeMutable(Slot);
	if (!Runtime)
	{
		return false;
	}

	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		Inventory.PrimaryWeaponClass = WeaponClass;
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		Inventory.SecondaryWeaponClass = WeaponClass;
	}

	ClearRuntimeWeapon(Inventory, *Runtime);
	Runtime->WeaponActor = SpawnWeapon(Inventory, WeaponClass);
	Runtime->InClip = Runtime->WeaponActor ? FMath::Max(Runtime->WeaponActor->GetClipSize(), 0) : -1;

	if (Runtime->WeaponActor)
	{
		BindRuntimeWeaponDelegates(Inventory, Slot, Runtime->WeaponActor);
	}

	SyncAttachments(Inventory);
	Inventory.OnItemSlotUpdated.Broadcast(Slot);
	Inventory.OnInventoryStateChanged.Broadcast();
	return Runtime->WeaponActor != nullptr || !WeaponClass;
}

void FPlayerInventoryWeaponRuntime::SyncAttachments(const UPlayerInventoryComponent& Inventory)
{
	const FName PrimaryHolsterSocket = GetHolsterSocketForSlot(Inventory, EPlayerItemSlot::PrimaryWeapon);
	const FName SecondaryHolsterSocket = GetHolsterSocketForSlot(Inventory, EPlayerItemSlot::SecondaryWeapon);
	const bool bHasPrimaryItem = Inventory.HasValidWeaponItem(EPlayerItemSlot::PrimaryWeapon);
	const bool bHasSecondaryItem = Inventory.HasValidWeaponItem(EPlayerItemSlot::SecondaryWeapon);
	const bool bHasActiveSlottedWeapon = Inventory.HasValidWeaponSelection(Inventory.ActiveWeaponSlot);

	auto SyncWeaponPresentation = [](AWeaponBase* Weapon, bool bShouldShow)
	{
		if (!Weapon)
		{
			return;
		}

		Weapon->SetActorHiddenInGame(!bShouldShow);
		Weapon->SetActorEnableCollision(bShouldShow);
		if (!bShouldShow)
		{
			Weapon->StopFire();
		}
	};

	if (!Inventory.bWeaponEquipped)
	{
		SyncWeaponPresentation(Inventory.PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
		SyncWeaponPresentation(Inventory.SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
		SyncWeaponPresentation(Inventory.UnarmedWeapon, false);
		if (bHasPrimaryItem)
		{
			AttachWeapon(Inventory, Inventory.PrimaryWeaponRuntime.WeaponActor, PrimaryHolsterSocket, true);
		}
		if (bHasSecondaryItem)
		{
			AttachWeapon(Inventory, Inventory.SecondaryWeaponRuntime.WeaponActor, SecondaryHolsterSocket, true);
		}
		return;
	}

	const bool bPrimaryActive = Inventory.ActiveWeaponSlot == EPlayerItemSlot::PrimaryWeapon;
	SyncWeaponPresentation(Inventory.PrimaryWeaponRuntime.WeaponActor, bHasPrimaryItem);
	SyncWeaponPresentation(Inventory.SecondaryWeaponRuntime.WeaponActor, bHasSecondaryItem);
	SyncWeaponPresentation(Inventory.UnarmedWeapon, !bHasActiveSlottedWeapon);
	if (bHasPrimaryItem)
	{
		AttachWeapon(Inventory, Inventory.PrimaryWeaponRuntime.WeaponActor,
			bPrimaryActive ? Inventory.WeaponHandSocketName : PrimaryHolsterSocket,
			!bPrimaryActive);
	}
	if (bHasSecondaryItem)
	{
		AttachWeapon(Inventory, Inventory.SecondaryWeaponRuntime.WeaponActor,
			bPrimaryActive ? SecondaryHolsterSocket : Inventory.WeaponHandSocketName,
			bPrimaryActive);
	}
	if (!bHasActiveSlottedWeapon && Inventory.UnarmedWeapon)
	{
		AttachWeapon(Inventory, Inventory.UnarmedWeapon, Inventory.WeaponHandSocketName, false);
	}
}

AWeaponBase* FPlayerInventoryWeaponRuntime::SpawnWeapon(const UPlayerInventoryComponent& Inventory,
                                                        TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass)
	{
		return nullptr;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(Inventory.GetOwner());
	UWorld* World = Inventory.GetWorld();
	if (!OwnerCharacter || !World)
	{
		return nullptr;
	}

	AWeaponBase* SpawnedWeapon = World->SpawnActor<AWeaponBase>(WeaponClass);
	if (!SpawnedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInventoryComponent '%s' failed to spawn weapon class '%s'."),
			*Inventory.GetName(), *GetNameSafe(WeaponClass));
		return nullptr;
	}

	SpawnedWeapon->SetOwner(OwnerCharacter);
	return SpawnedWeapon;
}

void FPlayerInventoryWeaponRuntime::ClearRuntimeWeapon(UPlayerInventoryComponent& Inventory,
                                                       FPlayerInventoryWeaponSlotRuntime& Runtime)
{
	if (!Runtime.WeaponActor)
	{
		return;
	}

	Runtime.WeaponActor->OnWeaponFiredEvent().RemoveAll(&Inventory);
	Runtime.WeaponActor->OnWeaponReloadedEvent().RemoveAll(&Inventory);
	Runtime.WeaponActor->Destroy();
	Runtime.WeaponActor = nullptr;
	Runtime.InClip = -1;
	Runtime.bIsReloading = false;
}

void FPlayerInventoryWeaponRuntime::BindRuntimeWeaponDelegates(UPlayerInventoryComponent& Inventory, EPlayerItemSlot Slot,
                                                               AWeaponBase* WeaponActor)
{
	if (!WeaponActor)
	{
		return;
	}

	WeaponActor->OnWeaponFiredEvent().RemoveAll(&Inventory);
	WeaponActor->OnWeaponReloadedEvent().RemoveAll(&Inventory);
	if (Slot == EPlayerItemSlot::PrimaryWeapon)
	{
		WeaponActor->OnWeaponFiredEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnPrimaryWeaponFired);
		WeaponActor->OnWeaponReloadedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnPrimaryWeaponReloaded);
	}
	else if (Slot == EPlayerItemSlot::SecondaryWeapon)
	{
		WeaponActor->OnWeaponFiredEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnSecondaryWeaponFired);
		WeaponActor->OnWeaponReloadedEvent().AddUObject(&Inventory, &UPlayerInventoryComponent::OnSecondaryWeaponReloaded);
	}
}

void FPlayerInventoryWeaponRuntime::AttachWeapon(const UPlayerInventoryComponent& Inventory, AWeaponBase* Weapon,
                                                 FName SocketName, bool bUseHolsterOffset)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(Inventory.GetOwner());
	if (!Weapon || !OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	Weapon->AttachToComponent(OwnerCharacter->GetMesh(), Rules, SocketName);
	Weapon->SetActorRelativeTransform(Weapon->GetAttachmentOffset(bUseHolsterOffset));
}

FName FPlayerInventoryWeaponRuntime::GetHolsterSocketForSlot(const UPlayerInventoryComponent& Inventory,
                                                             EPlayerItemSlot WeaponSlot)
{
	switch (WeaponSlot)
	{
	case EPlayerItemSlot::PrimaryWeapon:
		return Inventory.PrimaryWeaponHolsterSocketName;
	case EPlayerItemSlot::SecondaryWeapon:
		return Inventory.SecondaryWeaponHolsterSocketName;
	default:
		return NAME_None;
	}
}
