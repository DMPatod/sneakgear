#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBase.generated.h"

UCLASS(Abstract)
class SNEAKGEAR_API UWidgetBase : public UUserWidget
{
public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(EditAnywhere)
	uint16 Value = 0;

private:
	GENERATED_BODY()
};
