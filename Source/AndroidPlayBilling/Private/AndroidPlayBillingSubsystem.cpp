// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#include "AndroidPlayBillingSubsystem.h"

#if PLATFORM_ANDROID

#include "AndroidPlayBilling.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

// SkuDetails class fields
static jfieldID SkuDetailsRecord_OriginalJSON_FieldID;
static jfieldID SkuDetailsRecord_ProductID_FieldID;
static jfieldID SkuDetailsRecord_Type_FieldID;
static jfieldID SkuDetailsRecord_Price_FieldID;
static jfieldID SkuDetailsRecord_PriceAmountMicros_FieldID;
static jfieldID SkuDetailsRecord_PriceCurrencyCode_FieldID;
static jfieldID SkuDetailsRecord_OriginalPrice_FieldID;
static jfieldID SkuDetailsRecord_OriginalPriceAmountMicros_FieldID;
static jfieldID SkuDetailsRecord_Title_FieldID;
static jfieldID SkuDetailsRecord_Description_FieldID;
static jfieldID SkuDetailsRecord_SubscriptionPeriod_FieldID;
static jfieldID SkuDetailsRecord_FreeTrialPeriod_FieldID;
static jfieldID SkuDetailsRecord_IntroductoryPrice_FieldID;
static jfieldID SkuDetailsRecord_IntroductoryPriceAmountMicros_FieldID;
static jfieldID SkuDetailsRecord_IntroductoryPricePeriod_FieldID;
static jfieldID SkuDetailsRecord_IntroductoryPriceCycles_FieldID;
static jfieldID SkuDetailsRecord_IconURL_FieldID;

// PurchaseRecordFromHistory class fields
static jfieldID PurchaseRecordFromHistory_ProductID_FieldID;
static jfieldID PurchaseRecordFromHistory_PurchaseTime_FieldID;
static jfieldID PurchaseRecordFromHistory_PurchaseToken_FieldID;
static jfieldID PurchaseRecordFromHistory_DeveloperPayload_FieldID;
static jfieldID PurchaseRecordFromHistory_OriginalJSON_FieldID;
static jfieldID PurchaseRecordFromHistory_Signature_FieldID;

// PurchaseRecord class fields
static jfieldID PurchaseRecord_OrderID_FieldID;
static jfieldID PurchaseRecord_PackageName_FieldID;
static jfieldID PurchaseRecord_ProductID_FieldID;
static jfieldID PurchaseRecord_PurchaseTime_FieldID;
static jfieldID PurchaseRecord_PurchaseToken_FieldID;
static jfieldID PurchaseRecord_PurchaseState_FieldID;
static jfieldID PurchaseRecord_DeveloperPayload_FieldID;
static jfieldID PurchaseRecord_bAcknowledged_FieldID;
static jfieldID PurchaseRecord_bAutoRenewing_FieldID;
static jfieldID PurchaseRecord_OriginalJSON_FieldID;
static jfieldID PurchaseRecord_Signature_FieldID;
static jfieldID PurchaseRecord_ObfuscatedAccountID_FieldID;
static jfieldID PurchaseRecord_ObfuscatedProfileID_FieldID;

// Google Play Billing methods
static jmethodID LaunchBillingFlow_MethodID;
static jmethodID LaunchPriceChangeConfirmationFlow_MethodID;
static jmethodID QueryPurchases_MethodID;
static jmethodID QuerySkuDetails_MethodID;
static jmethodID QueryPurchaseHistory_MethodID;
static jmethodID IsFeatureSupported_MethodID;
static jmethodID IsIAPInitialized_MethodID;
static jmethodID IsPurchased_MethodID;

static inline FScopedJavaObject<jstring> GetScopedStringFromObject(
	JNIEnv* Env, 
	jobject Object, 
	jfieldID FieldID)
{
	return NewScopedJavaObject(Env, (jstring) Env->GetObjectField(Object, FieldID));
}

static jmethodID FindMethod(
	JNIEnv* Env, 
	const char* Name, 
	const char* Signature)
{
	if (Env && Name && Signature)
	{
		return Env->GetMethodID(FJavaWrapper::GameActivityClassID, Name, Signature);
	}

	return nullptr;
}

static jfieldID FindField(
	JNIEnv* Env, 
	jclass Class, 
	const char* Name, 
	const char* Type)
{
	if (Env && Name && Type && Class)
	{
		return Env->GetFieldID(Class, Name, Type);
	}
	
	return nullptr;
}

static bool CallBooleanMethod(JNIEnv* Env, jmethodID Method, ...)
{
	// make sure the function exists
	jobject Object = FJavaWrapper::GameActivityThis;
	if (Method == NULL || Object == NULL || Env == NULL)
	{
		return false;
	}

	va_list Args;
	va_start(Args, Method);
	return Env->CallBooleanMethodV(Object, Method, Args);
	va_end(Args);
}

static bool CallIntMethod(JNIEnv* Env, jmethodID Method, ...)
{
	// make sure the function exists
	jobject Object = FJavaWrapper::GameActivityThis;
	if (Method == NULL || Object == NULL || Env == NULL)
	{
		return false;
	}

	va_list Args;
	va_start(Args, Method);
	return Env->CallIntMethodV(Object, Method, Args);
	va_end(Args);
}

static void CallVoidMethod(JNIEnv* Env, jmethodID Method, ...)
{
	// make sure the function exists
	jobject Object = FJavaWrapper::GameActivityThis;
	if (Method == NULL || Object == NULL || Env == NULL)
	{
		return;
	}

	va_list Args;
	va_start(Args, Method);
	Env->CallVoidMethodV(Object, Method, Args);
	va_end(Args);
}

static jobject CallObjectMethod(JNIEnv* Env, jmethodID Method)
{
	// make sure the function exists
	jobject Object = FJavaWrapper::GameActivityThis;
	if (Method == NULL || Object == NULL || Env == NULL)
	{
		return NULL;
	}

	return Env->CallObjectMethod(Object, Method);
}

FPurchaseRecord GetPurchaseRecordStructFromJavaObject(JNIEnv* Env, jobject Object)
{
	if (Object == NULL || Env == NULL)
	{
		return {};
	}
	
	// PurchaseRecord class fields
	auto JOrderID					= GetScopedStringFromObject(Env, Object, PurchaseRecord_OrderID_FieldID);
	auto JPackageName				= GetScopedStringFromObject(Env, Object, PurchaseRecord_PackageName_FieldID);
	auto JProductID					= GetScopedStringFromObject(Env, Object, PurchaseRecord_ProductID_FieldID);
	int64 PurchaseTime				= (int64)Env->GetLongField(Object, PurchaseRecord_PurchaseTime_FieldID);
	auto JPurchaseToken				= GetScopedStringFromObject(Env, Object, PurchaseRecord_PurchaseToken_FieldID);
	EPurchaseState PurchaseState	= (EPurchaseState)Env->GetIntField(Object, PurchaseRecord_PurchaseState_FieldID);
	auto JDeveloperPayload			= GetScopedStringFromObject(Env, Object, PurchaseRecord_DeveloperPayload_FieldID);
	bool bAcknowledged				= (bool)Env->GetBooleanField(Object, PurchaseRecord_bAcknowledged_FieldID);
	bool bAutoRenewing				= (bool)Env->GetBooleanField(Object, PurchaseRecord_bAutoRenewing_FieldID);
	auto JOriginalJSON				= GetScopedStringFromObject(Env, Object, PurchaseRecord_OriginalJSON_FieldID);
	auto JSignature					= GetScopedStringFromObject(Env, Object, PurchaseRecord_Signature_FieldID);
	auto JObfuscatedAccountID		= GetScopedStringFromObject(Env, Object, PurchaseRecord_ObfuscatedAccountID_FieldID);
	auto JObfuscatedProfileID		= GetScopedStringFromObject(Env, Object, PurchaseRecord_ObfuscatedProfileID_FieldID);

	// Construct FPurchaseRecord struct
	return
	{
		FJavaHelper::FStringFromParam(Env, *JOrderID),
		FJavaHelper::FStringFromParam(Env, *JPackageName),
		FJavaHelper::FStringFromParam(Env, *JProductID),
		PurchaseTime,
		FJavaHelper::FStringFromParam(Env, *JPurchaseToken),
		PurchaseState,
		FJavaHelper::FStringFromParam(Env, *JDeveloperPayload),
		bAcknowledged,
		bAutoRenewing,
		FJavaHelper::FStringFromParam(Env, *JOriginalJSON),
		FJavaHelper::FStringFromParam(Env, *JSignature),
		FJavaHelper::FStringFromParam(Env, *JObfuscatedAccountID),
		FJavaHelper::FStringFromParam(Env, *JObfuscatedProfileID)
	};
}

FPurchaseRecordFromHistory GetPurchaseRecordFromHistoryStructFromJavaObject(JNIEnv* Env, jobject Object)
{
	if (Object == NULL || Env == NULL)
	{
		return {};
	}

	// PurchaseRecordFromHistory class fields
	auto JProductID			= GetScopedStringFromObject(Env, Object, PurchaseRecordFromHistory_ProductID_FieldID);
	int64 PurchaseTime		= (int64)Env->GetLongField(Object, PurchaseRecordFromHistory_PurchaseTime_FieldID);
	auto JPurchaseToken		= GetScopedStringFromObject(Env, Object, PurchaseRecordFromHistory_PurchaseToken_FieldID);
	auto JDeveloperPayload	= GetScopedStringFromObject(Env, Object, PurchaseRecordFromHistory_DeveloperPayload_FieldID);
	auto JOriginalJSON		= GetScopedStringFromObject(Env, Object, PurchaseRecordFromHistory_OriginalJSON_FieldID);
	auto JSignature			= GetScopedStringFromObject(Env, Object, PurchaseRecordFromHistory_Signature_FieldID);

	// Construct FPurchaseRecordFromHistory struct
	return
	{
		FJavaHelper::FStringFromParam(Env, *JProductID),
		PurchaseTime,
		FJavaHelper::FStringFromParam(Env, *JPurchaseToken),
		FJavaHelper::FStringFromParam(Env, *JDeveloperPayload),
		FJavaHelper::FStringFromParam(Env, *JOriginalJSON),
		FJavaHelper::FStringFromParam(Env, *JSignature),
	};
}

FSkuDetailsRecord GetSkuDetailsRecordStructFromJavaObject(JNIEnv* Env, jobject Object)
{
	if (Object == NULL || Env == NULL)
	{
		return {};
	}

	// SkuDetailsRecord class fields
	auto JOriginalJSON					= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_OriginalJSON_FieldID);
	auto JProductID						= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_ProductID_FieldID);
	auto JType							= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_Type_FieldID);
	auto JPrice							= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_Price_FieldID);
	int64 PriceAmountMicros				= (int64)Env->GetLongField(Object, SkuDetailsRecord_PriceAmountMicros_FieldID);
	auto JPriceCurrencyCode				= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_PriceCurrencyCode_FieldID);
	auto JOriginalPrice					= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_OriginalPrice_FieldID);
	int64 OriginalPriceAmountMicros		= (int64)Env->GetLongField(Object, SkuDetailsRecord_OriginalPriceAmountMicros_FieldID);
	auto JTitle							= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_Title_FieldID);
	auto JDescription					= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_Description_FieldID);
	auto JSubscriptionPeriod			= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_SubscriptionPeriod_FieldID);
	auto JFreeTrialPeriod				= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_FreeTrialPeriod_FieldID);
	auto JIntroductoryPrice				= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_IntroductoryPrice_FieldID);
	int64 IntroductoryPriceAmountMicros	= (int64)Env->GetLongField(Object, SkuDetailsRecord_IntroductoryPriceAmountMicros_FieldID);
	auto JIntroductoryPricePeriod		= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_IntroductoryPricePeriod_FieldID);
	int IntroductoryPriceCycles			= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_IntroductoryPriceCycles_FieldID);
	auto JIconURL						= GetScopedStringFromObject(Env, Object, SkuDetailsRecord_IconURL_FieldID);

	// Construct FSkuDetailsRecord struct
	return
	{
		FJavaHelper::FStringFromParam(Env, *JOriginalJSON),
		FJavaHelper::FStringFromParam(Env, *JProductID),
		FJavaHelper::FStringFromParam(Env, *JType),
		FJavaHelper::FStringFromParam(Env, *JPrice),
		PriceAmountMicros,
		FJavaHelper::FStringFromParam(Env, *JPriceCurrencyCode),
		FJavaHelper::FStringFromParam(Env, *JOriginalPrice),
		OriginalPriceAmountMicros,
		FJavaHelper::FStringFromParam(Env, *JTitle),
		FJavaHelper::FStringFromParam(Env, *JDescription),
		FJavaHelper::FStringFromParam(Env, *JSubscriptionPeriod),
		FJavaHelper::FStringFromParam(Env, *JFreeTrialPeriod),
		FJavaHelper::FStringFromParam(Env, *JIntroductoryPrice),
		IntroductoryPriceAmountMicros,
		FJavaHelper::FStringFromParam(Env, *JIntroductoryPricePeriod),
		IntroductoryPriceCycles,
		FJavaHelper::FStringFromParam(Env, *JIconURL),
	};
}

#endif

void UAndroidPlayBillingSubsystem::LaunchBillingFlow(
	FOnLaunchBillingFlowCompleted OnOperationResult,
	const FString& ProductID,
	const ESkuType ProductType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
		{
			// Bind callback
			Module->AddOnLaunchBillingFlowCompleted_Handle(OnOperationResult);

			auto JProductID = FJavaHelper::ToJavaString(Env, ProductID);
			
			// Call Java method
			CallVoidMethod(
				Env,
				LaunchBillingFlow_MethodID,
				*JProductID,
				(int)ProductType);
		}
	}
#endif
}

void UAndroidPlayBillingSubsystem::LaunchPriceChangeConfirmationFlow(
	FOnOperationCompleted OnOperationResult,
	const FString& ProductID,
	const ESkuType ProductType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
		{
			// Bind callback
			Module->AddOnOperationCompleted_Handle(OnOperationResult);

			auto JProductID = FJavaHelper::ToJavaString(Env, ProductID);
			
			// Call Java method
			CallVoidMethod(
				Env,
				LaunchPriceChangeConfirmationFlow_MethodID,
				*JProductID,
				(int)ProductType);
		}
	}
#endif
}

void UAndroidPlayBillingSubsystem::QueryPurchases(
	FOnQueryPurchasesCompleted OnOperationResult,
	const ESkuType ProductType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
		{
			// Bind callback
			Module->AddOnQueryPurchasesCompleted_Handle(OnOperationResult);

			// Call Java method
			CallVoidMethod(Env, QueryPurchases_MethodID, (int)ProductType);
		}
	}
#endif
}

void UAndroidPlayBillingSubsystem::QuerySkuDetails(
	FOnQuerySkuDetailsCompleted OnOperationResult,
	const TArray<FString>& ProductIDs,
	const ESkuType ProductType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
		{
			// Bind callback
			Module->AddOnQuerySkuDetailsCompleted_Handle(OnOperationResult);

			// Populate some java types with the provided product information
			auto JProductIDs = NewScopedJavaObject(
				Env,
				(jobjectArray)Env->NewObjectArray(ProductIDs.Num(),
				FJavaWrapper::JavaStringClass,
				NULL));
			
			for (int Idx = 0; Idx < ProductIDs.Num(); Idx++)
			{
				auto JProductID = FJavaHelper::ToJavaString(Env, ProductIDs[Idx]);
				Env->SetObjectArrayElement(*JProductIDs, Idx, *JProductID);
			}

			// Call Java method
			CallVoidMethod(
				Env,
				QuerySkuDetails_MethodID,
				*JProductIDs,
				(int)ProductType);
		}
	}
#endif
}

void UAndroidPlayBillingSubsystem::QueryPurchaseHistory(
	FOnQueryPurchaseHistoryCompleted OnOperationResult,
	const ESkuType ProductType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
		{
			// Bind callback
			Module->AddOnQueryPurchaseHistoryCompleted_Handle(OnOperationResult);

			// Call Java method
			CallVoidMethod(Env, QueryPurchaseHistory_MethodID, (int)ProductType);
		}
	}
#endif
}

EBillingResponseCode UAndroidPlayBillingSubsystem::IsFeatureSupported(const EFeatureType FeatureType)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		// Call Java method
		return (EBillingResponseCode) CallIntMethod(Env, IsFeatureSupported_MethodID, (int) FeatureType);
	}
#endif

	return EBillingResponseCode::SERVICE_UNAVAILABLE;
}

bool UAndroidPlayBillingSubsystem::IsIAPInitialized()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		// Call Java method
		return CallBooleanMethod(Env, IsIAPInitialized_MethodID);
	}
#endif

	return false;
}

bool UAndroidPlayBillingSubsystem::IsPurchased(const FString& ProductID)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		auto JProductID = FJavaHelper::ToJavaString(Env, ProductID);
		
		// Call Java method
		return CallBooleanMethod(Env, IsIAPInitialized_MethodID, *JProductID);
	}
#endif

	return false;
}

#if PLATFORM_ANDROID
JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeInitialize(
	JNIEnv* Env, 
	jobject Thiz,
	jclass SkuDetailsRecordClass,
	jclass PurchaseRecordFromHistoryClass,
	jclass PurchaseRecordClass)
{
	// SkuDetailsRecord class fields
	SkuDetailsRecord_OriginalJSON_FieldID					= FindField(Env, SkuDetailsRecordClass, "OriginalJSON",					"Ljava/lang/String;");
	SkuDetailsRecord_ProductID_FieldID						= FindField(Env, SkuDetailsRecordClass, "ProductID",					"Ljava/lang/String;");
	SkuDetailsRecord_Type_FieldID							= FindField(Env, SkuDetailsRecordClass, "Type",							"Ljava/lang/String;");
	SkuDetailsRecord_Price_FieldID							= FindField(Env, SkuDetailsRecordClass, "Price",						"Ljava/lang/String;");
	SkuDetailsRecord_PriceAmountMicros_FieldID				= FindField(Env, SkuDetailsRecordClass, "PriceAmountMicros",			"J");
	SkuDetailsRecord_PriceCurrencyCode_FieldID				= FindField(Env, SkuDetailsRecordClass, "PriceCurrencyCode",			"Ljava/lang/String;");
	SkuDetailsRecord_OriginalPrice_FieldID					= FindField(Env, SkuDetailsRecordClass, "OriginalPrice",				"Ljava/lang/String;");
	SkuDetailsRecord_OriginalPriceAmountMicros_FieldID		= FindField(Env, SkuDetailsRecordClass, "OriginalPriceAmountMicros",	"J");
	SkuDetailsRecord_Title_FieldID							= FindField(Env, SkuDetailsRecordClass, "Title",						"Ljava/lang/String;");
	SkuDetailsRecord_Description_FieldID					= FindField(Env, SkuDetailsRecordClass, "Description",					"Ljava/lang/String;");
	SkuDetailsRecord_SubscriptionPeriod_FieldID				= FindField(Env, SkuDetailsRecordClass, "SubscriptionPeriod",			"Ljava/lang/String;");
	SkuDetailsRecord_FreeTrialPeriod_FieldID				= FindField(Env, SkuDetailsRecordClass, "FreeTrialPeriod",				"Ljava/lang/String;");
	SkuDetailsRecord_IntroductoryPrice_FieldID				= FindField(Env, SkuDetailsRecordClass, "IntroductoryPrice",			"Ljava/lang/String;");
	SkuDetailsRecord_IntroductoryPriceAmountMicros_FieldID	= FindField(Env, SkuDetailsRecordClass, "IntroductoryPriceAmountMicros","J");
	SkuDetailsRecord_IntroductoryPricePeriod_FieldID		= FindField(Env, SkuDetailsRecordClass, "IntroductoryPricePeriod",		"Ljava/lang/String;");
	SkuDetailsRecord_IntroductoryPriceCycles_FieldID		= FindField(Env, SkuDetailsRecordClass, "IntroductoryPriceCycles",		"I");
	SkuDetailsRecord_IconURL_FieldID						= FindField(Env, SkuDetailsRecordClass, "IconURL",						"Ljava/lang/String;");
	
	// PurchaseRecordFromHistory class fields
	PurchaseRecordFromHistory_ProductID_FieldID			= FindField(Env, PurchaseRecordFromHistoryClass, "ProductID",		"Ljava/lang/String;");
	PurchaseRecordFromHistory_PurchaseTime_FieldID		= FindField(Env, PurchaseRecordFromHistoryClass, "PurchaseTime",	"J");
	PurchaseRecordFromHistory_PurchaseToken_FieldID		= FindField(Env, PurchaseRecordFromHistoryClass, "PurchaseToken",	"Ljava/lang/String;");
	PurchaseRecordFromHistory_DeveloperPayload_FieldID	= FindField(Env, PurchaseRecordFromHistoryClass, "DeveloperPayload","Ljava/lang/String;");
	PurchaseRecordFromHistory_OriginalJSON_FieldID		= FindField(Env, PurchaseRecordFromHistoryClass, "OriginalJSON",	"Ljava/lang/String;");
	PurchaseRecordFromHistory_Signature_FieldID			= FindField(Env, PurchaseRecordFromHistoryClass, "Signature",		"Ljava/lang/String;");
	
	// PurchaseRecord class fields
	PurchaseRecord_OrderID_FieldID				= FindField(Env, PurchaseRecordClass, "OrderID",			"Ljava/lang/String;");
	PurchaseRecord_PackageName_FieldID			= FindField(Env, PurchaseRecordClass, "PackageName",		"Ljava/lang/String;");
	PurchaseRecord_ProductID_FieldID			= FindField(Env, PurchaseRecordClass, "ProductID",			"Ljava/lang/String;");
	PurchaseRecord_PurchaseTime_FieldID			= FindField(Env, PurchaseRecordClass, "PurchaseTime",		"J");
	PurchaseRecord_PurchaseToken_FieldID		= FindField(Env, PurchaseRecordClass, "PurchaseToken",		"Ljava/lang/String;");
	PurchaseRecord_PurchaseState_FieldID		= FindField(Env, PurchaseRecordClass, "PurchaseState",		"I");
	PurchaseRecord_DeveloperPayload_FieldID		= FindField(Env, PurchaseRecordClass, "DeveloperPayload",	"Ljava/lang/String;");
	PurchaseRecord_bAcknowledged_FieldID		= FindField(Env, PurchaseRecordClass, "bAcknowledged",		"Z");
	PurchaseRecord_bAutoRenewing_FieldID		= FindField(Env, PurchaseRecordClass, "bAutoRenewing",		"Z");
	PurchaseRecord_OriginalJSON_FieldID			= FindField(Env, PurchaseRecordClass, "OriginalJSON",		"Ljava/lang/String;");
	PurchaseRecord_Signature_FieldID			= FindField(Env, PurchaseRecordClass, "Signature",			"Ljava/lang/String;");
	PurchaseRecord_ObfuscatedAccountID_FieldID	= FindField(Env, PurchaseRecordClass, "ObfuscatedAccountID","Ljava/lang/String;");
	PurchaseRecord_ObfuscatedProfileID_FieldID	= FindField(Env, PurchaseRecordClass, "ObfuscatedProfileID","Ljava/lang/String;");
	
	// Find Google Play Billing methods
	LaunchBillingFlow_MethodID					= FindMethod(Env, "AndroidThunkJava_LaunchBillingFlow",						"(Ljava/lang/String;I)V");
	LaunchPriceChangeConfirmationFlow_MethodID	= FindMethod(Env, "AndroidThunkJava_LaunchPriceChangeConfirmationFlow",		"(Ljava/lang/String;I)V");
	QueryPurchases_MethodID						= FindMethod(Env, "AndroidThunkJava_QueryPurchases",						"(I)V");
	QuerySkuDetails_MethodID					= FindMethod(Env, "AndroidThunkJava_QuerySkuDetails",						"([Ljava/lang/String;I)V");
	QueryPurchaseHistory_MethodID				= FindMethod(Env, "AndroidThunkJava_QueryPurchaseHistory",					"(I)V");
	IsFeatureSupported_MethodID					= FindMethod(Env, "AndroidThunkJava_IsFeatureSupported",					"(I)I");
	IsIAPInitialized_MethodID					= FindMethod(Env, "AndroidThunkJava_IsIAPInitialized",						"()Z");
	IsPurchased_MethodID						= FindMethod(Env, "AndroidThunkJava_IsPurchased",							"(Ljava/lang/String;)Z");
}

JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeCompleteOperation(
	JNIEnv* Env, 
	jobject Thiz, 
	jint ResponseCode,
	jstring ResponseMessage)
{
	if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
	{
		Module->TriggerOnOperationCompleted
		(
			(EBillingResponseCode)ResponseCode,
			FJavaHelper::FStringFromParam(Env, ResponseMessage)
		);
	}
}

JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeCompleteLaunchBillingFlow(
	JNIEnv* Env, 
	jobject Thiz, 
	jint ResponseCode,
	jstring ResponseMessage,
	jobjectArray Array,
	jint ArraySize)
{
	if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
	{
		// Convert java record
		TArray<FPurchaseRecord> PurchaseRecords = {};
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			// Get element from java array
			auto JPurchaseRecord = NewScopedJavaObject(Env, Env->GetObjectArrayElement(Array, Idx));

			// Construct struct & add to array
			PurchaseRecords.Add(GetPurchaseRecordStructFromJavaObject(Env, *JPurchaseRecord));
		}

		Module->TriggerOnLaunchBillingFlowCompleted
		(
			(EBillingResponseCode)ResponseCode,
			FJavaHelper::FStringFromParam(Env, ResponseMessage),
			PurchaseRecords
		);
	}
}

JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeCompleteQueryPurchaseHistory(
	JNIEnv* Env, 
	jobject Thiz, 
	jint ResponseCode,
	jstring ResponseMessage,
	jobjectArray Array,
	jint ArraySize)
{
	if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
	{
		// Convert java record
		TArray<FPurchaseRecordFromHistory> PurchaseRecordsFromHistory = {};
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			// Get element from java array
			auto JRecordFromHistory = NewScopedJavaObject(Env, Env->GetObjectArrayElement(Array, Idx));

			// Construct struct & add to array
			PurchaseRecordsFromHistory.Add(GetPurchaseRecordFromHistoryStructFromJavaObject(Env, *JRecordFromHistory));
		}

		Module->TriggerOnQueryPurchaseHistoryCompleted
		(
			(EBillingResponseCode)ResponseCode,
			FJavaHelper::FStringFromParam(Env, ResponseMessage),
			PurchaseRecordsFromHistory
		);
	}
}

JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeCompleteQuerySkuDetails(
	JNIEnv* Env, 
	jobject Thiz, 
	jint ResponseCode,
	jstring ResponseMessage,
	jobjectArray Array,
	jint ArraySize)
{
	if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
	{
		// Convert java record
		TArray<FSkuDetailsRecord> SkuDetailsRecords = {};
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			// Get element from java array
			auto JSkuDetailsRecord = NewScopedJavaObject(Env, Env->GetObjectArrayElement(Array, Idx));

			// Construct struct & add to array
			SkuDetailsRecords.Add(GetSkuDetailsRecordStructFromJavaObject(Env, *JSkuDetailsRecord));
		}

		Module->TriggerOnQuerySkuDetailsCompleted
		(
			(EBillingResponseCode)ResponseCode,
			FJavaHelper::FStringFromParam(Env, ResponseMessage),
			SkuDetailsRecords
		);
	}
}

JNI_METHOD void Java_com_kulichin_androidbilling_AndroidBilling_NativeCompleteQueryPurchases(
	JNIEnv* Env, 
	jobject Thiz, 
	jint ResponseCode,
	jstring ResponseMessage,
	jobjectArray Array,
	jint ArraySize)
{
	if (FAndroidPlayBillingModule* Module = FAndroidPlayBillingModule::GetModule())
	{
		// Convert java record
		TArray<FPurchaseRecord> PurchaseRecords = {};
		for (int Idx = 0; Idx < ArraySize; Idx++)
		{
			// Get element from java array
			auto JPurchaseRecord = NewScopedJavaObject(Env, Env->GetObjectArrayElement(Array, Idx));

			// Construct struct & add to array
			PurchaseRecords.Add(GetPurchaseRecordStructFromJavaObject(Env, *JPurchaseRecord));
		}

		Module->TriggerOnQueryPurchasesCompleted
		(
			(EBillingResponseCode)ResponseCode,
			FJavaHelper::FStringFromParam(Env, ResponseMessage),
			PurchaseRecords
		);
	}
}

#endif

