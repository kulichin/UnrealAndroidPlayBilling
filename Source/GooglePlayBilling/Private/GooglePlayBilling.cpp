// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "GooglePlayBilling.h"
#include "GooglePlayBillingSettings.h"
#include "Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FGooglePlayBillingModule"

void FGooglePlayBillingModule::StartupModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			TEXT("Project"), 
			TEXT("Plugins"), 
			TEXT("Google Billing"),
			LOCTEXT("Google Billing", "Google Billing"),
			LOCTEXT("Google Billing", "Settings for Google Billing"),
			GetMutableDefault<UGooglePlayBillingSettings>());
	}
}

void FGooglePlayBillingModule::ShutdownModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(
			TEXT("Project"),
			TEXT("Plugins"),
			TEXT("GooglePlayBilling"));
	}
}

FGooglePlayBillingModule* FGooglePlayBillingModule::GetModule()
{
	return FModuleManager::Get().GetModulePtr<FGooglePlayBillingModule>("GooglePlayBilling");
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FGooglePlayBillingModule, GooglePlayBilling)
