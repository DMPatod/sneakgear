#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIntStatUpdated, int32, OldValue, int32, NewValue, int32, MaxValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFloatStatUpdated, float, OldValue, float, NewValue, float, MaxValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerIsDead);

class UInputAction;
class UItemDefinition;
class AEquipableToolBase;
class UEquipableToolDefinition;
class UInventoryComponent;

UCLASS(Abstract)
class SNEAKGEAR_API APlayerCharacterBase : public ACharacter
{
public:
	APlayerCharacterBase();

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void GiveItem(UItemDefinition* ItemDefinition);

	UPROPERTY(BlueprintAssignable, Category = "Player|Health")
	FIntStatUpdated OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Stamina")
	FFloatStatUpdated OnStaminaChanged;

	UFUNCTION(BlueprintCallable, Category = "Player|Stats")
	void BroadcastCurrentStats();

	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void ToggleSprint();
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Player|Health")
	int GetHealth();

	UFUNCTION(BlueprintCallable, Category = "Player|Health")
	void UpdateHealth(int DeltaHealth);

	UFUNCTION(BlueprintCallable, Category = "Player|Health")
	void SetMaxHealth(int NewMaxHealth);

	UPROPERTY(BlueprintAssignable, Category = "Player|Health")
	FPlayerIsDead OnPlayerDied;

	UFUNCTION(BlueprintPure, Category = "Player|Stamina")
	float GetStamina();

	UFUNCTION(BlueprintCallable, Category = "Player|Stamina")
	float GetStaminaRegenRate();

	UFUNCTION(BlueprintCallable, Category = "Player|Stamina")
	void SetStaminaRegenRate(float NewStaminaRegenRate);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Movement", meta = (AllowPrivateAccess = "true"))
	float NormalMaxWalkSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Movement", meta = (AllowPrivateAccess = "true"))
	float SprintingMaxWalkSpeed = 800.0f;

	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void SetSprinting(bool IsSprinting);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UseAction;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UFUNCTION()
	bool IsToolAlreadyOwned(UEquipableToolDefinition* ToolDefinition);

	UFUNCTION()
	void AttachTool(UEquipableToolDefinition* ToolDefinition);

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TObjectPtr<AEquipableToolBase> EquippedTool;

private:
	static constexpr int BaseStatValue = 3;
	int MaxHealth = BaseStatValue;
	int CurrentHealth = BaseStatValue;

	static constexpr float MaxStamina = 100.0f;
	float CurrentStamina = MaxStamina;
	float StaminaRegenRate = 1.0f;
	static constexpr float SprintStaminaCost = 5.0f;
	static constexpr float RestStaminaRegenRate = 2.0f;

	bool bIsCrouched = false;
	bool bIsSprinting = false;
	bool bHasSprinted = false;

	GENERATED_BODY()
};
