#include "EquipableToolDefinition.h"

#include "EquipableToolBase.h"

UEquipableToolDefinition* UEquipableToolDefinition::CreateItemCopy() const
{
	UEquipableToolDefinition* ItemCopy = NewObject<UEquipableToolDefinition>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemText = this->ItemText;
	ItemCopy->WorldMesh = this->WorldMesh;
	ItemCopy->ToolAsset = this->ToolAsset;

	return ItemCopy;
}
