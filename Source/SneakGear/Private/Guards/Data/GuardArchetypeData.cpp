#include "Guards/Data/GuardArchetypeData.h"

#include "Misc/DataValidation.h"

EDataValidationResult UGuardArchetypeData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (VisionRange <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("VisionRange must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (HearingRange <= 0.f)
	{
		Context.AddError(FText::FromString(TEXT("HearingRange must be greater than 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (VisionHalfAngleDeg <= 0.f || VisionHalfAngleDeg > 180.f)
	{
		Context.AddError(FText::FromString(TEXT("VisionHalfAngleDeg must be in the range (0, 180].")));
		Result = EDataValidationResult::Invalid;
	}

	if (ReactionTimeSeconds < 0.f)
	{
		Context.AddError(FText::FromString(TEXT("ReactionTimeSeconds must be greater than or equal to 0.")));
		Result = EDataValidationResult::Invalid;
	}

	if (AimErrorDegrees < 0.f || AimErrorDegrees > 45.f)
	{
		Context.AddError(FText::FromString(TEXT("AimErrorDegrees must be in the range [0, 45].")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
