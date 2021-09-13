// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidPlayBilling.h"
#include "AndroidPlayBillingSettings.h"
#include "Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FAndroidPlayBillingModule"

void FAndroidPlayBillingModule::StartupModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			TEXT("Project"), 
			TEXT("Plugins"), 
			TEXT("Android Play Billing"),
			LOCTEXT("Android Play Billing", "Android Play Billing"),
			LOCTEXT("Android Play Billing", "Settings for Android Play Billing"),
			GetMutableDefault<UAndroidPlayBillingSettings>());
	}
}

void FAndroidPlayBillingModule::ShutdownModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(
			TEXT("Project"),
			TEXT("Plugins"),
			TEXT("AndroidPlayBilling"));
	}
}

FAndroidPlayBillingModule* FAndroidPlayBillingModule::GetModule()
{
	return FModuleManager::Get().GetModulePtr<FAndroidPlayBillingModule>("AndroidPlayBilling");
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FAndroidPlayBillingModule, AndroidPlayBilling)
