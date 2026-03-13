#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/PlayerInventoryTypes.h"
#include "WeaponQuickIndicatorWidget.generated.h"

class UTextBlock;

UCLASS()
class SNEAKGEARUI_API UWeaponQuickIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UI|Weapons")
	void UpdateForSlot(EPlayerItemSlot ItemSlot);

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AmmoText;

private:
	FText GetSlotLabel(EPlayerItemSlot Slot) const;
	const class IPlayerUIDataSource* GetPlayerUIDataSource() const;
};
