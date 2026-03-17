#include "Items/PlayerItemPickupComponent.h"

#include "Items/PlayerItemDefinition.h"
#include "Items/WorldItemPickup.h"
#include "Misc/DataValidation.h"
#include "Weapon/WeaponBase.h"

EDataValidationResult UPlayerItemPickupComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ItemDefinition)
	{
		const AActor* OwnerActor = GetOwner();
		const AWorldItemPickup* WorldPickup = Cast<AWorldItemPickup>(OwnerActor);
		const bool bOwnerProvidesItemDefinition = WorldPickup && WorldPickup->GetItemDefinition() != nullptr;
		if (!bOwnerProvidesItemDefinition)
		{
			Context.AddError(FText::FromString(TEXT("ItemDefinition must be assigned or provided by the owning WorldItemPickup.")));
			Result = EDataValidationResult::Invalid;
		}
	}
	else if (!ItemDefinition->BuildInventoryItem().IsValid())
	{
		Context.AddError(FText::FromString(TEXT("Assigned ItemDefinition does not build a valid inventory item.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

FPlayerInventoryItem UPlayerItemPickupComponent::GetPickupItem() const
{
	return ItemDefinition ? ItemDefinition->BuildInventoryItem() : FPlayerInventoryItem();
}

TSubclassOf<AWeaponBase> UPlayerItemPickupComponent::GetPickupWeaponClass() const
{
	return ItemDefinition ? ItemDefinition->WeaponClass : nullptr;
}

UPlayerItemDefinition* UPlayerItemPickupComponent::GetItemDefinition() const
{
	return ItemDefinition;
}

void UPlayerItemPickupComponent::SetItemDefinition(UPlayerItemDefinition* InItemDefinition)
{
	ItemDefinition = InItemDefinition;
}

void UPlayerItemPickupComponent::ConsumePickup()
{
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->Destroy();
	}
}
