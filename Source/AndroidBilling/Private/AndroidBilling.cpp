// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidBilling.h"
#include "AndroidBillingSettings.h"
#include "Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FGooglePlayBillingModule"

void FAndroidBillingModule::StartupModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			TEXT("Project"), 
			TEXT("Plugins"), 
			TEXT("Android Billing"),
			LOCTEXT("Android Billing", "Android Billing"),
			LOCTEXT("Android Billing", "Settings for Android Billing"),
			GetMutableDefault<UAndroidBillingSettings>());
	}
}

void FAndroidBillingModule::ShutdownModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(
			TEXT("Project"),
			TEXT("Plugins"),
			TEXT("AndroidBilling"));
	}
}

FAndroidBillingModule* FAndroidBillingModule::GetModule()
{
	return FModuleManager::Get().GetModulePtr<FAndroidBillingModule>("AndroidBilling");
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FAndroidBillingModule, AndroidBilling)
