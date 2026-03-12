#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/Components/PlayerItemComponent.h"
#include "WeaponQuickIndicatorWidget.generated.h"

class UTextBlock;
class AStealthPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UWeaponQuickIndicatorWidget : public UUserWidget
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
	AStealthPlayerCharacter* GetOwningStealthCharacter() const;
};
