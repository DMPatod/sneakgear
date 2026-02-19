#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponStatusWidget.generated.h"

class UTextBlock;
class AThirdPersonPlayerCharacter;

UCLASS()
class SNEAKGEAR_API UWeaponStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> FireRateText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AmmoText;

private:
	TWeakObjectPtr<AThirdPersonPlayerCharacter> CachedPlayer;

	void UpdateFromPlayer();
};
