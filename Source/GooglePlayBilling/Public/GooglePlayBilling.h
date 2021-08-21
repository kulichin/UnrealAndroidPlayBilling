// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#pragma once

#include "GooglePlayBillingSubsystem.h"
#include "Engine.h"

#define INITIALIZE_DELEGATE_HANDLER_Base(DelegateClass, DelegateName)	\
public:																	\
	DelegateClass DelegateName;											\
	void Add##DelegateName##_Handle(const DelegateClass& Delegate)		\
	{																	\
		ClearAll##DelegateName();										\
		DelegateName = Delegate;										\
	}																	\
	void ClearAll##DelegateName()										\
	{																	\
		if(DelegateName.IsBound())										\
		{																\
			DelegateName.Clear();										\
		}																\
	}

#define INITIALIZE_DELEGATE_HANDLER_TwoParams(															\
		DelegateClass, DelegateName,																	\
		FirstParamType, FirstParamName, SecondParamType, SecondParamName)								\
		INITIALIZE_DELEGATE_HANDLER_Base(DelegateClass, DelegateName)									\
	void Trigger##DelegateName(FirstParamType FirstParamName, SecondParamType SecondParamName)			\
	{																									\
		if (DelegateName.IsBound())																		\
		{																								\
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(										\
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([FirstParamName, SecondParamName, this]()	\
			{																							\
				DelegateName.Execute(FirstParamName, SecondParamName);									\
				ClearAll##DelegateName();																\
			}),																							\
			TStatId(), nullptr, ENamedThreads::GameThread);												\
		}																								\
	}

#define INITIALIZE_DELEGATE_HANDLER_ThreeParams(																				\
		DelegateClass, DelegateName,																							\
		FirstParamType, FirstParamName, SecondParamType, SecondParamName, ThirdParamType, ThirdParamName)						\
		INITIALIZE_DELEGATE_HANDLER_Base(DelegateClass, DelegateName)															\
	void Trigger##DelegateName(FirstParamType FirstParamName, SecondParamType SecondParamName, ThirdParamType ThirdParamName)	\
	{																															\
		if (DelegateName.IsBound())																								\
		{																														\
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(																\
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([FirstParamName, SecondParamName, ThirdParamName, this]()			\
			{																													\
				DelegateName.Execute(FirstParamName, SecondParamName, ThirdParamName);											\
				ClearAll##DelegateName();																						\
			}),																													\
			TStatId(), nullptr, ENamedThreads::GameThread);																		\
		}																														\
	}

class FGooglePlayBillingModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	static FGooglePlayBillingModule* GetModule();

#if PLATFORM_ANDROID
	INITIALIZE_DELEGATE_HANDLER_TwoParams(
		FOnOperationCompleted, OnOperationCompleted,
		const EBillingResponseCode, ResponseCode,
		const FString&, ResponseMessage)
	
	INITIALIZE_DELEGATE_HANDLER_ThreeParams(
		FOnLaunchBillingFlowCompleted, OnLaunchBillingFlowCompleted,
		const EBillingResponseCode, ResponseCode,
		const FString&, ResponseMessage,
		const TArray<FPurchaseRecord>&, UpdatedPurchaseRecords);
	
	INITIALIZE_DELEGATE_HANDLER_ThreeParams(
		FOnQueryPurchaseHistoryCompleted, OnQueryPurchaseHistoryCompleted,
		const EBillingResponseCode, ResponseCode,
		const FString&, ResponseMessage,
		const TArray<FPurchaseRecordFromHistory>&, PurchaseRecordsFromHistory);
	
	INITIALIZE_DELEGATE_HANDLER_ThreeParams(
		FOnQuerySkuDetailsCompleted, OnQuerySkuDetailsCompleted,
		const EBillingResponseCode, ResponseCode,
		const FString&, ResponseMessage,
		const TArray<FSkuDetailsRecord>&, SkuDetailsRecords);
	
	INITIALIZE_DELEGATE_HANDLER_ThreeParams(
		FOnQueryPurchasesCompleted, OnQueryPurchasesCompleted,
		const EBillingResponseCode, ResponseCode,
		const FString&, ResponseMessage,
		const TArray<FPurchaseRecord>&, PurchaseRecords);
#endif
};
