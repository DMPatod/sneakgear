#include "UI/WidgetBase.h"

#if WITH_EDITOR

const FText UWidgetBase::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "Customs Widgets");
}

#endif