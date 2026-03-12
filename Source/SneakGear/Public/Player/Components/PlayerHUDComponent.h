#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/RadarTypes.h"
#include "PlayerHUDComponent.generated.h"

class AStealthPlayerController;
class AStealthPlayerCharacter;
class UCrosshairWidget;
class UMainHUDWidget;
class URadarWidget;

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

	void Initialize(TSubclassOf<UMainHUDWidget> InMainHUDWidgetClass, TSubclassOf<UCrosshairWidget> InCrosshairWidgetClass,
	                const FCrosshairSpreadConfig& InCrosshairSpread);

	void OnWeaponFired();
	void SetCrosshairVisible(bool bVisible) const;
	void NotifyHitMarker() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CreateHUDWidgets();
	void UpdateRadarWidget();
	void UpdateCrosshairWidget(float DeltaSeconds);
	AStealthPlayerController* GetOwningStealthPlayerController() const;
	AStealthPlayerCharacter* GetOwningStealthPlayerCharacter() const;
	URadarWidget* GetRadarWidget() const;

	UPROPERTY(Transient)
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	UPROPERTY(Transient)
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UMainHUDWidget> MainHUDWidget;

	UPROPERTY(Transient)
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	FCrosshairSpreadConfig CrosshairSpread;
	TArray<FRadarContact> RadarContactsCache;
	float RadarRefreshCooldown = 0.f;
	float RadarRefreshInterval = 0.1f;
	float SpreadCurrent = 0.f;
	float SpreadTarget = 0.f;
};
