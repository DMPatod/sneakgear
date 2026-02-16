#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WaypointAction.generated.h"

class AAIController;
class APawn;

UINTERFACE(BlueprintType)
class SNEAKGEAR_API UWaypointAction : public UInterface
{
	GENERATED_BODY()
};

class SNEAKGEAR_API IWaypointAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Patrol")
	float ExecuteAtWaypoint(APawn* GuardPawn, AAIController* GuardController);
};

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class SNEAKGEAR_API UWaypointActionBase : public UObject, public IWaypointAction
{
	GENERATED_BODY()

public:
	virtual float ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController) override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SNEAKGEAR_API UWaypointAction_StandBy : public UWaypointActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol", meta=(ClampMin="0.0"))
	float Duration = 1.f;

	virtual float ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController) override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SNEAKGEAR_API UWaypointAction_LookAround : public UWaypointActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Patrol", meta=(ClampMin="0.0"))
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, Category="Patrol", meta=(ClampMin="0.0"))
	float YawDelta = 45.f;

	virtual float ExecuteAtWaypoint_Implementation(APawn* GuardPawn, AAIController* GuardController) override;
};
