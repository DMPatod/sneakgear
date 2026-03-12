#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Guards/BehaviorTree/BTTask_SetNextPatrolPoint.h"
#include "Guards/GuardAIController.h"
#include "Guards/GuardCharacter.h"
#include "Guards/Components/GuardPatrolComponent.h"
#include "Player/Components/PlayerItemComponent.h"
#include "Player/ThirdPersonPlayerCharacter.h"
#include "UI/EventFeedWidget.h"
#include "UI/PlayerVitalsWidget.h"
#include "UI/StanceWidget.h"
#include "Weapon/WeaponAimProvider.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/WeaponFireModeComponent.h"
#include "SneakGearTestTypes.generated.h"

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

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEARTESTS_API UTestPlayerItemComponent : public UPlayerItemComponent
{
	GENERATED_BODY()

public:
	void ConfigureWeaponClasses(TSubclassOf<AWeaponBase> InPrimaryWeaponClass, TSubclassOf<AWeaponBase> InSecondaryWeaponClass = nullptr);
	void RunBeginPlayForTest();
};

UCLASS()
class SNEAKGEARTESTS_API UTestWeaponFireModeComponent : public UWeaponFireModeComponent
{
	GENERATED_BODY()

public:
	virtual void FireOnce(const FWeaponFireContext& Context) override;

	int32 FireCount = 0;
	FWeaponFireContext LastContext;
};

UCLASS()
class SNEAKGEARTESTS_API ATestWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ATestWeapon();

	UTestWeaponFireModeComponent* GetTestFireMode() const;
};

UCLASS()
class SNEAKGEARTESTS_API ATestInventoryCharacter : public AThirdPersonPlayerCharacter
{
	GENERATED_BODY()

public:
	ATestInventoryCharacter();

	UTestPlayerItemComponent* GetTestItemComponent() const;
	void InitializeAbilitySystemForTest(float InitialAmmo, float MaxAmmo);
	virtual AWeaponBase* GetCurrentWeapon() const override;

private:
	UPROPERTY(VisibleAnywhere, Category="Tests")
	TObjectPtr<UTestPlayerItemComponent> TestItemComponent;
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

protected:
	virtual void BeginPlay() override;
};

UCLASS()
class SNEAKGEARTESTS_API ATestGuardAIController : public AGuardAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

	bool InitializeBlackboardForTest(UBlackboardData* BlackboardAsset);
	void UpdateBlackboardForTest();
};

UCLASS()
class SNEAKGEARTESTS_API UTestBTTask_SetNextPatrolPoint : public UBTTask_SetNextPatrolPoint
{
	GENERATED_BODY()

public:
	void ConfigureBlackboardKeys(FName InPatrolLocationKeyName, FName InPatrolIndexKeyName);
	void SetLoopingForTest(bool bInLoop);
};

UCLASS()
class SNEAKGEARTESTS_API UTestPlayerVitalsWidget : public UPlayerVitalsWidget
{
	GENERATED_BODY()

public:
	FText GetHealthDisplayText() const;
	FText GetStaminaDisplayText() const;
};

UCLASS()
class SNEAKGEARTESTS_API UTestStanceWidget : public UStanceWidget
{
	GENERATED_BODY()

public:
	void ConstructForTest();
	FText GetStanceDisplayText() const;
};

UCLASS()
class SNEAKGEARTESTS_API UTestEventFeedWidget : public UEventFeedWidget
{
	GENERATED_BODY()

public:
	FText GetEventFeedText() const;
};
