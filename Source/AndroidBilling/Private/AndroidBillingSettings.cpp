// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidBillingSettings.h"

#if WITH_EDITOR
void UAndroidBillingSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}
#endif
