#include "Data/ActorDefinition.h"

UActorDefinition* UActorDefinition::CreateActorCopy() const
{
	auto* Copy = NewObject<UActorDefinition>(StaticClass());

	Copy->ID = this->ID;
	Copy->ActorType = this->ActorType;
	Copy->ActorText = this->ActorText;
	Copy->WorldMesh = this->WorldMesh;
	
	return Copy;
}
