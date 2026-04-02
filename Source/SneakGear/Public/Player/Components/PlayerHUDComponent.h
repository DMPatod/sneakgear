#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/RadarTypes.h"
#include "PlayerHUDComponent.generated.h"

class ASneakGearPlayerController;
class ASneakGearPlayerCharacter;
class UCrosshairWidget;
class UInfoPromptNativeWidget;
class UPlayerHUDWidget;
class URadarWidget;
class UUserWidget;

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

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UPlayerHUDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerHUDComponent();

	void Initialize(TSubclassOf<UPlayerHUDWidget> InPlayerHUDWidgetClass, TSubclassOf<UCrosshairWidget> InCrosshairWidgetClass,
	                const FCrosshairSpreadConfig& InCrosshairSpread, bool bInShowCrosshairOnlyWhenAiming);

	void OnWeaponFired();
	void SetCrosshairVisible(bool bVisible) const;
	void NotifyHitMarker() const;
	void SetOverlayWidgetVisible(TSubclassOf<UUserWidget> WidgetClass, bool bVisible);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CreateHUDWidgets();
	void UpdateRadarWidget();
	void UpdateCrosshairWidget(float DeltaSeconds);
	void UpdateVaultPromptWidget();
	void UpdateCoverDebugWidget();
	ASneakGearPlayerController* GetOwningSneakGearPlayerController() const;
	ASneakGearPlayerCharacter* GetOwningSneakGearPlayerCharacter() const;
	URadarWidget* GetRadarWidget() const;

	UPROPERTY(Transient)
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(Transient)
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(Transient)
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UUserWidget>> OverlayWidgets;

	UPROPERTY(Transient)
	TSubclassOf<UInfoPromptNativeWidget> VaultPromptWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UInfoPromptNativeWidget> VaultPromptWidget;

	TObjectPtr<UInfoPromptNativeWidget> CoverDebugWidget;

	FCrosshairSpreadConfig CrosshairSpread;
	TArray<FRadarContact> RadarContactsCache;
	float RadarRefreshCooldown = 0.f;
	float RadarRefreshInterval = 0.1f;
	int32 VaultPromptZOrder = 6;
	int32 CoverDebugWidgetZOrder = 7;
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bShowCoverDebugWidget = false;
	float SpreadCurrent = 0.f;
	float SpreadTarget = 0.f;
	bool bShowCrosshairOnlyWhenAiming = false;
};
