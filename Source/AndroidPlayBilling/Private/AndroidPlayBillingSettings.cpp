// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidPlayBillingSettings.h"

#if WITH_EDITOR
void UAndroidPlayBillingSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}
#endif
