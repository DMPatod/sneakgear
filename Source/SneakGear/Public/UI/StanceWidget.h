#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/ThirdPersonPlayerCharacter.h"
#include "StanceWidget.generated.h"

class AThirdPersonPlayerCharacter;
class UTextBlock;
class UImage;
enum class EStance : uint8;

UCLASS()
class SNEAKGEAR_API UStanceWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> StanceImage;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> StanceText;

	UPROPERTY(EditDefaultsOnly, Category="Stances")
	TMap<EStance, TObjectPtr<UTexture2D>> StanceTextures;

private:
	TWeakObjectPtr<AThirdPersonPlayerCharacter> CachedPlayer;
	EStance CurrentStance = EStance::Standing;
	bool bHasInitializedUI = false;

	bool TryCachePlayer();
	void HandleStanceChanged(EStance NewStance);
	void UpdateFromPlayer();
};
