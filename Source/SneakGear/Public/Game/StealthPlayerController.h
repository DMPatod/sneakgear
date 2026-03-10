#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/PlayerItemComponent.h"
#include "StealthPlayerController.generated.h"

class UCrosshairWidget;
class UMainHUDWidget;
class UInputMappingContext;
class URadarWidget;
class UWeaponQuickIndicatorWidget;
class UWeaponSelectionMenuWidget;

USTRUCT()
struct FCrosshairSpreadConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float Min = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float Max = 30.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float InterpolationSpeed = 15.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float FromMove = 22.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float FromAimMultiplier = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Spread")
	float ShootKick = 10.f;
};

UCLASS()
class SNEAKGEAR_API AStealthPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStealthPlayerController();
	virtual void BeginPlay() override;

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
	void SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot);

	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void CancelWeaponSelectionMenu();

	UFUNCTION(BlueprintPure, Category="UI|Weapons")
	bool IsWeaponSelectionWidgetOpen() const
	{
		return bWeaponSelectionOpen;
	}

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponQuickSelectIndicator(EPlayerItemSlot Slot);

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponSelectionOpened(EPlayerItemSlot InitialSlot);

	UFUNCTION(BlueprintImplementableEvent, Category="UI|Weapons")
	void BP_OnWeaponSelectionClosed();

protected:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UInputMappingContext> DebugMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	TSubclassOf<UWeaponQuickIndicatorWidget> WeaponQuickIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	TSubclassOf<UWeaponSelectionMenuWidget> WeaponSelectionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI|Weapons")
	float WeaponQuickIndicatorDuration = 0.9f;

	UPROPERTY()
	TObjectPtr<UMainHUDWidget> MainHUDWidget;

	UPROPERTY()
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	UPROPERTY(Transient)
	TObjectPtr<UWeaponQuickIndicatorWidget> WeaponQuickIndicatorWidget;

	UPROPERTY(Transient)
	TObjectPtr<UWeaponSelectionMenuWidget> WeaponSelectionWidget;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair")
	FCrosshairSpreadConfig CrosshairSpread;

private:
	void SetupInputMappings();
	void CreateHUDWidgets();
	void UpdateRadarWidget();
	void UpdateCrosshairWidget(float DeltaSeconds);

	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebug = false;

	float SpreadCurrent = 0.f;
	float SpreadTarget = 0.f;
	bool bWeaponSelectionOpen = false;
	bool bWasGamePausedBeforeWeaponSelection = false;
	FTimerHandle WeaponQuickIndicatorTimer;
};
