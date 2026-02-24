#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

class UCanvasPanelSlot;
class UImage;

UCLASS()
class SNEAKGEAR_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetVisible(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void SetSpread(float InSpread);

	UFUNCTION(BlueprintCallable)
	void ShowHitMarker(float Duration = 0.08f);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Crosshair_Up;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Crosshair_Down;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Crosshair_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Crosshair_Right;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> HitMarker;

private:
	TObjectPtr<UCanvasPanelSlot> Slot_Up, Slot_Down, Slot_Left, Slot_Right = nullptr;
	FVector2D BasePosUp, BasePosDown, BasePosLeft, BasePosRight;
	FTimerHandle HitMarkerTimer;

	void HideHitMarker();
};
