#pragma once

#include "CoreMinimal.h"
#include "UI/EventFeedWidget.h"
#include "UI/PlayerVitalsWidget.h"
#include "UI/StanceWidget.h"
#include "TestUIWidgets.generated.h"

UCLASS()
class SNEAKGEARTESTS_API UTestPlayerVitalsWidget : public UPlayerVitalsWidget
{
	GENERATED_BODY()

public:
	FText GetHealthDisplayText() const;
	FText GetStaminaDisplayText() const;
};

UCLASS()
class SNEAKGEARTESTS_API UTestStanceWidget : public UStanceWidget
{
	GENERATED_BODY()

public:
	void ConstructForTest();
	FText GetStanceDisplayText() const;
};

UCLASS()
class SNEAKGEARTESTS_API UTestEventFeedWidget : public UEventFeedWidget
{
	GENERATED_BODY()

public:
	FText GetEventFeedText() const;
};
