#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UEquipableToolDefinition;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SNEAKGEAR_API UInventoryComponent : public UActorComponent
{

public:
	UInventoryComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TArray<UEquipableToolDefinition*> ToolInventory;

protected:
	virtual void BeginPlay() override;

private:
	GENERATED_BODY()

};
