#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GuardPatrolComponent.generated.h"

class APatrolPath;
class AController;

UCLASS(ClassGroup=(SneakGear), meta=(BlueprintSpawnableComponent))
class SNEAKGEAR_API UGuardPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGuardPatrolComponent();

	void SetPatrolPath(APatrolPath* NewPatrolPath);
	APatrolPath* GetPatrolPath() const;
	void ApplyToController(AController* InController) const;

#if WITH_EDITOR
	void DrawEditorPreview() const;
#endif

private:
	UPROPERTY(EditInstanceOnly, Category="Patrol")
	TObjectPtr<APatrolPath> PatrolPath;

	UPROPERTY(EditInstanceOnly, Category="Patrol|Editor")
	bool bShowPatrolPathInEditor = false;

	UPROPERTY(EditInstanceOnly, Category="Patrol|Editor", meta=(ClampMin="0.0"))
	float PatrolPathPreviewDuration = 60.f;
};
