// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidPlayBilling.h"
#include "Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FAndroidPlayBillingModule"

void FAndroidPlayBillingModule::StartupModule() {}
void FAndroidPlayBillingModule::ShutdownModule() {}

FAndroidPlayBillingModule* FAndroidPlayBillingModule::GetModule()
{
	return FModuleManager::Get().GetModulePtr<FAndroidPlayBillingModule>("AndroidPlayBilling");
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FAndroidPlayBillingModule, AndroidPlayBilling)
