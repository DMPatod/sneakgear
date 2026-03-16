#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupPromptWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class SNEAKGEARUI_API UPickupPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetPickupInfo(const FText& ItemName, const FText& SlotLabel);

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetSwapPrompt(bool bInRequiresHoldToSwap);

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetHoldProgress(float InProgress);

	UFUNCTION(BlueprintCallable, Category="UI|Pickup")
	void SetPromptVisible(bool bVisible);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Pickup")
	FText PromptFormat = NSLOCTEXT("SneakGearUI", "PickupPromptFormat", "Pick up {0}");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Pickup")
	FText SwapPromptFormat = NSLOCTEXT("SneakGearUI", "PickupSwapPromptFormat", "Hold to swap {0}");

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PromptText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> HoldProgressBar;

private:
	FText CachedItemName;
	FText CachedSlotLabel;
	float CachedHoldProgress = 0.f;
	bool bPromptVisible = false;
	bool bRequiresHoldToSwap = false;

	void ApplyCachedState();
	void EnsureRuntimeWidgetTree();
};
