#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/Components/PlayerHUDComponent.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "UI/WeaponMenuActions.h"
#include "StealthPlayerController.generated.h"

class UInputMappingContext;
class UPlayerHUDComponent;
class UPlayerWeaponSelectionComponent;
class UUserWidget;
class UWeaponQuickIndicatorWidget;
class UWeaponSelectionMenuWidget;

UCLASS()
class SNEAKGEAR_API AStealthPlayerController : public APlayerController, public IWeaponMenuActions
{
	GENERATED_BODY()

public:
	AStealthPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void OnWeaponFired();

	UFUNCTION(BlueprintCallable)
	void SetCrosshairVisible(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void NotifyHitMarker();

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot);

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot);

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void CloseWeaponSelectionWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	virtual void SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot) override;

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	virtual void CancelWeaponSelectionMenu() override;

	UFUNCTION(BlueprintPure, Category="UI|Weapons")
	bool IsWeaponSelectionWidgetOpen() const;

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponQuickSelectIndicator(EPlayerItemSlot Slot);

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponSelectionOpened(EPlayerItemSlot InitialSlot);

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponSelectionClosed();

	void ApplyMenuInputMode(UUserWidget* FocusWidget);
	void ApplyGameplayInputMode();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DebugMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	TSubclassOf<UWeaponQuickIndicatorWidget> WeaponQuickIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	TSubclassOf<UWeaponSelectionMenuWidget> WeaponSelectionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	float WeaponQuickIndicatorDuration = 0.9f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair")
	FCrosshairSpreadConfig CrosshairSpread;

private:
	void SetupInputMappings();

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPlayerHUDComponent> PlayerHUDComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPlayerWeaponSelectionComponent> PlayerWeaponSelectionComponent;

	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebug = false;
};
