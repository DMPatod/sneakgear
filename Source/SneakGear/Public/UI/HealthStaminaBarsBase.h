#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "HealthStaminaBarsBase.generated.h"

class UStatBarBase;

UCLASS(Abstract)
class SNEAKGEAR_API UHealthStaminaBarsBase : public UWidgetBase
{
public:
	UFUNCTION()
	void SetDetails(bool Details);

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	TObjectPtr<UStatBarBase> HealthBar = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Constituent Controls", meta = (BindWidget))
	TObjectPtr<UStatBarBase> StaminaBar = nullptr;

private:
	GENERATED_BODY()
};
