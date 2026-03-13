#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponStatusWidget.generated.h"

class UTextBlock;
class APawn;

UCLASS()
class SNEAKGEARUI_API UWeaponStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> FireRateText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> AmmoText;

private:
	TWeakObjectPtr<APawn> CachedPlayer;

	void HandleInventoryStateChanged();
	const class IPlayerUIDataSource* GetPlayerUIDataSource() const;
	void UpdateFromPlayer();
};
