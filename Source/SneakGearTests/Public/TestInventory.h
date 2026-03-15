#pragma once

#include "CoreMinimal.h"
#include "Items/EquipmentItemDefinition.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Items/UtilityItemDefinition.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "TestInventory.generated.h"

class AWeaponBase;
class APlayerCharacterBase;
class UPlayerItemDefinition;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEARTESTS_API UTestPlayerInventoryComponent : public UPlayerInventoryComponent
{
	GENERATED_BODY()

public:
	void ConfigureWeaponClasses(TSubclassOf<AWeaponBase> InPrimaryWeaponClass, TSubclassOf<AWeaponBase> InSecondaryWeaponClass = nullptr);
	void RunBeginPlayForTest();
};

UCLASS()
class SNEAKGEARTESTS_API UTestEquipmentItemDefinition : public UEquipmentItemDefinition
{
	GENERATED_BODY()

public:
	UTestEquipmentItemDefinition();

	virtual bool ActivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;
	virtual void DeactivateItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;

	mutable int32 ActivationCount = 0;
	mutable int32 DeactivationCount = 0;
};

UCLASS()
class SNEAKGEARTESTS_API UTestUtilityItemDefinition : public UUtilityItemDefinition
{
	GENERATED_BODY()

public:
	UTestUtilityItemDefinition();

	virtual bool UseItem(APlayerCharacterBase* PlayerCharacter, UPlayerInventoryComponent* InventoryComponent) const override;
	virtual bool ShouldConsumeOnUse() const override;

	UPROPERTY()
	bool bUseSucceeded = true;

	UPROPERTY()
	bool bConsumeAfterUseForTest = false;

	mutable int32 UseCount = 0;
};

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEARTESTS_API UTestPlayerItemPickupComponent : public UPlayerItemPickupComponent
{
	GENERATED_BODY()

public:
	void SetItemDefinitionForTest(UPlayerItemDefinition* InItemDefinition);
};

UCLASS()
class SNEAKGEARTESTS_API ATestPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ATestPickupActor();

	UTestPlayerItemPickupComponent* GetPickupComponent() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<class USphereComponent> Root;

	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<UTestPlayerItemPickupComponent> PickupComponent;
};
