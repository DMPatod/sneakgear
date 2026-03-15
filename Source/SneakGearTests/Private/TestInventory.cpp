#include "TestInventory.h"

#include "Components/SphereComponent.h"

void UTestPlayerInventoryComponent::ConfigureWeaponClasses(TSubclassOf<AWeaponBase> InPrimaryWeaponClass, TSubclassOf<AWeaponBase> InSecondaryWeaponClass)
{
	PrimaryWeaponClass = InPrimaryWeaponClass;
	SecondaryWeaponClass = InSecondaryWeaponClass;
}

void UTestPlayerInventoryComponent::RunBeginPlayForTest()
{
	BeginPlay();
}

UTestEquipmentItemDefinition::UTestEquipmentItemDefinition()
{
	ItemId = TEXT("TestEquipment");
	DisplayName = FText::FromString(TEXT("Test Equipment"));
	SlotType = EPlayerItemSlot::Equipped;
}

bool UTestEquipmentItemDefinition::ActivateItem(APlayerCharacterBase* PlayerCharacter,
                                                UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	++ActivationCount;
	return true;
}

void UTestEquipmentItemDefinition::DeactivateItem(APlayerCharacterBase* PlayerCharacter,
                                                  UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	++DeactivationCount;
}

UTestUtilityItemDefinition::UTestUtilityItemDefinition()
{
	ItemId = TEXT("TestUtility");
	DisplayName = FText::FromString(TEXT("Test Utility"));
	SlotType = EPlayerItemSlot::Utility;
}

bool UTestUtilityItemDefinition::UseItem(APlayerCharacterBase* PlayerCharacter,
                                         UPlayerInventoryComponent* InventoryComponent) const
{
	(void)PlayerCharacter;
	(void)InventoryComponent;
	++UseCount;
	return bUseSucceeded;
}

bool UTestUtilityItemDefinition::ShouldConsumeOnUse() const
{
	return bConsumeAfterUseForTest;
}

void UTestPlayerItemPickupComponent::SetItemDefinitionForTest(UPlayerItemDefinition* InItemDefinition)
{
	ItemDefinition = InItemDefinition;
}

ATestPickupActor::ATestPickupActor()
{
	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->InitSphereRadius(48.f);
	Root->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Root->SetCollisionObjectType(ECC_WorldDynamic);
	Root->SetCollisionResponseToAllChannels(ECR_Overlap);
	PickupComponent = CreateDefaultSubobject<UTestPlayerItemPickupComponent>(TEXT("PickupComponent"));
}

UTestPlayerItemPickupComponent* ATestPickupActor::GetPickupComponent() const
{
	return PickupComponent;
}
