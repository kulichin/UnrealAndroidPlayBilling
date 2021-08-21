// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "GooglePlayBillingSettings.h"

#if WITH_EDITOR
void UGooglePlayBillingSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}
#endif
