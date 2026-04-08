#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Guards/GuardCharacter.h"
#include "Guards/Components/GuardPatrolComponent.h"
#include "Player/PlayerCharacterBase.h"
#include "Weapon/WeaponAimProvider.h"
#include "TestCharacters.generated.h"

class AWeaponBase;
class UBoxComponent;
class UTestPlayerInventoryComponent;
class ATestDamageWeapon;

UCLASS()
class SNEAKGEARTESTS_API ATestDeathCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	void InitializeAbilitySystemForTest(float InitialHealth = 100.f);
	void BindHealthDeathForTest();
	void ApplyHealthDeltaForTest(float Delta);

	int32 DeathCount = 0;

protected:
	virtual void OnCharacterDeath() override;
};

UCLASS()
class SNEAKGEARTESTS_API ATestInventoryCharacter : public APlayerCharacterBase
{
	GENERATED_BODY()

public:
	ATestInventoryCharacter();

	UTestPlayerInventoryComponent* GetTestItemComponent() const;
	void InitializeAbilitySystemForTest(float InitialAmmo, float MaxAmmo);
	virtual AWeaponBase* GetCurrentWeapon() const override;

private:
	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<UTestPlayerInventoryComponent> TestItemComponent;
};

UCLASS()
class SNEAKGEARTESTS_API ATestAimPawn : public APawn, public IWeaponAimProvider
{
	GENERATED_BODY()

public:
	ATestAimPawn();

	virtual bool GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const override;

	bool bProvideAimData = true;
	FVector AimOrigin = FVector(10.f, 20.f, 30.f);
	FVector AimDirection = FVector::ForwardVector;
};

UCLASS()
class SNEAKGEARTESTS_API ATestPatrolPawn : public APawn
{
	GENERATED_BODY()

public:
	ATestPatrolPawn();

	UGuardPatrolComponent* GetPatrolComponent() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<UGuardPatrolComponent> PatrolComponent;
};

UCLASS()
class SNEAKGEARTESTS_API ATestGuardCharacter : public AGuardCharacter
{
	GENERATED_BODY()

public:
protected:
	ATestGuardCharacter();
	virtual void BeginPlay() override;
};

UCLASS()
class SNEAKGEARTESTS_API ATestCoverObstacle : public AActor
{
	GENERATED_BODY()

public:
	ATestCoverObstacle();

	void SetBoxExtent(const FVector& NewExtent);

private:
	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<UBoxComponent> BoxComponent;
};
