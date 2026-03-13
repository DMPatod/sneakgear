#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "PlayerWeaponSelectionComponent.generated.h"

class AStealthPlayerController;
class ASneakGearPlayerCharacter;
class UWeaponQuickIndicatorWidget;
class UWeaponSelectionMenuWidget;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerWeaponSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerWeaponSelectionComponent();

	void Initialize(TSubclassOf<UWeaponQuickIndicatorWidget> InWeaponQuickIndicatorWidgetClass,
	                TSubclassOf<UWeaponSelectionMenuWidget> InWeaponSelectionWidgetClass,
	                float InWeaponQuickIndicatorDuration);

	void ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot);
	void OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot);
	void CloseWeaponSelectionWidget();
	void SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot);
	void CancelWeaponSelectionMenu();

	bool IsWeaponSelectionWidgetOpen() const
	{
		return bWeaponSelectionOpen;
	}

protected:
	virtual void BeginPlay() override;

private:
	AStealthPlayerController* GetOwningStealthPlayerController() const;
	ASneakGearPlayerCharacter* GetOwningSneakGearPlayerCharacter() const;

	UPROPERTY(Transient)
	TSubclassOf<UWeaponQuickIndicatorWidget> WeaponQuickIndicatorWidgetClass;

	UPROPERTY(Transient)
	TSubclassOf<UWeaponSelectionMenuWidget> WeaponSelectionWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWeaponQuickIndicatorWidget> WeaponQuickIndicatorWidget;

	UPROPERTY(Transient)
	TObjectPtr<UWeaponSelectionMenuWidget> WeaponSelectionWidget;

	float WeaponQuickIndicatorDuration = 0.9f;
	bool bWeaponSelectionOpen = false;
	bool bWasGamePausedBeforeWeaponSelection = false;
	FTimerHandle WeaponQuickIndicatorTimer;
};
