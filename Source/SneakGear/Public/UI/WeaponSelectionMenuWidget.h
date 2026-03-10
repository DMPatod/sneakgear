#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PlayerItemComponent.h"
#include "WeaponSelectionMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UWeaponQuickIndicatorWidget;

UCLASS()
class SNEAKGEAR_API UWeaponSelectionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void InitializeMenu(EPlayerItemSlot InitialSlot);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> PrimaryWeaponButton;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> SecondaryWeaponButton;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> HeaderText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> EquippedItemText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SupportItemText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWeaponQuickIndicatorWidget> PrimaryWeaponInfoWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWeaponQuickIndicatorWidget> SecondaryWeaponInfoWidget;

private:
	void RefreshEntries(EPlayerItemSlot InitialSlot) const;
	void SetFocusedSlot(EPlayerItemSlot NewFocusedSlot);
	void ApplyFocusToCurrentSlot() const;
	void ConfirmCurrentSelection();

	UFUNCTION()
	void HandlePrimaryWeaponClicked();

	UFUNCTION()
	void HandleSecondaryWeaponClicked();

	UFUNCTION()
	void HandleCancelClicked();

	EPlayerItemSlot FocusedSelectionSlot = EPlayerItemSlot::PrimaryWeapon;
};
