// Copyright (C) 2021. Nikita Klimov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AndroidBillingSubsystem.generated.h"

UENUM(BlueprintType)
enum EBillingResponseCode
{
	/**
	 *	Billing API version is not supported for the type requested.
	 */
	BILLING_UNAVAILABLE		= 3		UMETA(DisplayName = "BillingUnavailable"),

	/**
	 *	Invalid arguments provided to the API.
	 */
	DEVELOPER_ERROR			= 5		UMETA(DisplayName = "DeveloperError"),

	/**
	 *	Fatal error during the API action.
	 */
	ERROR					= 6		UMETA(DisplayName = "Error"),

	/**
	 *	Requested feature is not supported by Play Store on the
	 *	current device.
	 */
	FEATURE_NOT_SUPPORTED	= -2	UMETA(DisplayName = "FeatureNotSupported"),

	/**
	 *	Failure to purchase since item is already owned.
	 */
	ITEM_ALREADY_OWNED		= 7		UMETA(DisplayName = "ItemAlreadyOwned"),

	/**
	 *	Failure to consume since item is not owned.
	 */
	ITEM_NOT_OWNED			= 8		UMETA(DisplayName = "ItemNotOwned"),

	/**
	 *	Requested product is not available for purchase.
	 */
	ITEM_UNAVAILABLE		= 4		UMETA(DisplayName = "ItemUnavailable"),

	/**
	 *	Success.
	 */
	OK						= 0		UMETA(DisplayName = "OK"),

	/**
	 *	Play Store service is not connected now - potentially
	 *	transient state.
	 */
	SERVICE_DISCONNECTED	= -1	UMETA(DisplayName = "ServiceDisconnected"),

	/**
	 *	The request has reached the maximum timeout before
	 *	Google Play responds.
	 */
	SERVICE_TIMEOUT			= -3	UMETA(DisplayName = "ServiceTimeout"),

	/**
	 *	Network connection is down.
	 */
	SERVICE_UNAVAILABLE		= 2		UMETA(DisplayName = "ServiceUnavailable"),

	/**
	 *	User pressed back or canceled a dialog.
	 */
	USER_CANCELED			= 1		UMETA(DisplayName = "UserCanceled")
};

UENUM(BlueprintType)
enum EFeatureType
{
	/**
	 *	Purchase/query for in-app items on VR.
	 */
	IN_APP_ITEMS_ON_VR			= 0 UMETA(DisplayName = "InAppPurchaseOnVR"),

	/**
	 *	Launch a price change confirmation flow.
	 */
	PRICE_CHANGE_CONFIRMATION	= 1 UMETA(DisplayName = "PriceChangeConfirmation"),

	/**
	 *	Purchase/query for subscriptions.
	 */
	SUBSCRIPTIONS				= 2 UMETA(DisplayName = "Subcriptions"),

	/**
	 *	Purchase/query for subscriptions on VR.
	 */
	SUBSCRIPTIONS_ON_VR			= 3 UMETA(DisplayName = "SubscriptionsOnVR"),

	/**
	 *	Subscriptions update/replace.
	 */
	SUBSCRIPTIONS_UPDATE		= 4 UMETA(DisplayName = "SubscriptionsUpdate")
};

UENUM(BlueprintType)
enum EPurchaseState
{
	PENDING				= 2 UMETA(DisplayName = "Pending"),
	PURCHASED			= 1 UMETA(DisplayName = "Purchased"),
	UNSPECIFIED_STATE	= 0 UMETA(DisplayName = "UnspecifiedState")
};

UENUM(BlueprintType)
enum ESkuType
{
	/**
	 *	A type of SKU for Android apps in-app products.
	 */
	INAPP	= 0 UMETA(DisplayName = "InAppProduct"),

	/**
	 *	A type of SKU for Android apps subscriptions.
	 */
	SUBS	= 1 UMETA(DisplayName = "InAppSubscription")
};

USTRUCT(BlueprintType)
struct FSkuDetailsRecord
{
	GENERATED_BODY()
	
	/**
	 *	Returns a String in JSON format that contains SKU details.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString OriginalJSON;
	
	/**
	 *	Returns the product Id.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString ProductID;
	
	/**
	 *	Returns the of the product.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString Type;
	
	/**
	 *	Returns formatted price of the item, including its currency sign.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString Price;
	
	/**
	 *	Returns price in micro-units, where 1,000,000 micro-units equal
	 *	one unit of the currency.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	int64 PriceAmountMicros;
	
	/**
	 *	Returns ISO 4217 currency code for price and original price.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString PriceCurrencyCode;
	
	/**
	 *	Returns formatted original price of the item, including its
	 *	currency sign.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString OriginalPrice;
	
	/**
	 *	Returns the original price in micro-units, where 1,000,000
	 *	micro-units equal one unit of the currency.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	int64 OriginalPriceAmountMicros;
	
	/**
	 *	Returns the title of the product being sold.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString Title;
	
	/**
	 *	Returns the description of the product.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString Description;
	
	/**
	 *	Subscription period, specified in ISO 8601 format.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString SubscriptionPeriod;
	
	/**
	 *	Trial period configured in Google Play Console, specified in
	 *	ISO 8601 format.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString FreeTrialPeriod;
	
	/**
	 *	Formatted introductory price of a subscription, including its
	 *	currency sign, such as €3.99.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString IntroductoryPrice;
	
	/**
	 *	Introductory price in micro-units.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	int64 IntroductoryPriceAmountMicros;
	
	/**
	 *	The billing period of the introductory price, specified in
	 *	ISO 8601 format.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString IntroductoryPricePeriod;
	
	/**
	 *	The number of subscription billing periods for which the user will
	 *	be given the introductory price, such as 3.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	int IntroductoryPriceCycles;
	
	/**
	 *	Returns the icon of the product if present.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "SkuDetailsRecord")
	FString IconURL;
};

USTRUCT(BlueprintType)
struct FPurchaseRecordFromHistory
{
	GENERATED_BODY()
	
	/**
	 *	Returns the product Id.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	FString ProductID;
	
	/**
	 *	Returns the time the product was purchased, in milliseconds since
	 *	the epoch (Jan 1, 1970).
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	int64 PurchaseTime;
	
	/**
	 *	Returns a token that uniquely identifies a purchase for a given
	 *	item and user pair.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	FString PurchaseToken;
	
	/**
	 *	Returns the payload specified when the purchase was acknowledged or
	 *	consumed.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	FString DeveloperPayload;
	
	/**
	 *	Returns a String in JSON format that contains details about the
	 *	purchase order.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	FString OriginalJSON;
	
	/**
	 *	Returns String containing the signature of the purchase data
	 *	that was signed with the private key of the developer.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecordFromHistory")
	FString Signature;
};

USTRUCT(BlueprintType)
struct FPurchaseRecord
{
	GENERATED_BODY()
	
	/**
	 *	Returns a unique order identifier for the transaction.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString OrderID;
	
	/**
	 *	Returns the application package from which the purchase originated.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString PackageName;
	
	/**
	 *	Returns the product Id.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString ProductID;
	
	/**
	 *	Returns the time the product was purchased, in milliseconds since
	 *	the epoch (Jan 1, 1970).
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	int64 PurchaseTime;
	
	/**
	 *	Returns a token that uniquely identifies a purchase for a given
	 *	item and user pair.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString PurchaseToken;
	
	/**
	 *	Returns PurchaseState indicating the state of the purchase.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	TEnumAsByte<EPurchaseState> PurchaseState;
	
	/**
	 *	Returns the payload specified when the purchase was acknowledged or
	 *	consumed.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString DeveloperPayload;
	
	/**
	 *	Indicates whether the purchase has been acknowledged.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	bool bAcknowledged;
	
	/**
	 *	Indicates whether the subscription renews automatically.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	bool bAutoRenewing;
	
	/**
	 *	Returns a String in JSON format that contains details about the
	 *	purchase order.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString OriginalJSON;
	
	/**
	 *	Returns String containing the signature of the purchase data that
	 *	was signed with the private key of the developer.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString Signature;
	
	/**
	 *	The obfuscated account id.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString ObfuscatedAccountID;

	/**
	 *	The obfuscated profile id.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PurchaseRecord")
	FString ObfuscatedProfileID;
};

/**
 *	Operation delegate
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnOperationCompleted,
	EBillingResponseCode, ResponseCode,
	const FString&, ResponseMessage);

/**
 *	Launch Billing Flow delegate
 */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FOnLaunchBillingFlowCompleted,
	EBillingResponseCode, ResponseCode,
	const FString&, ResponseMessage,
	const TArray<FPurchaseRecord>&, UpdatedPurchaseRecords);

/**
 *	Query Purchase History delegate
 */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FOnQueryPurchaseHistoryCompleted,
	EBillingResponseCode, ResponseCode,
	const FString&, ResponseMessage,
	const TArray<FPurchaseRecordFromHistory>&, PurchaseRecordsFromHistory);

/**
 *	Query Sku Details delegate
 */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FOnQuerySkuDetailsCompleted,
	EBillingResponseCode, ResponseCode,
	const FString&, ResponseMessage,
	const TArray<FSkuDetailsRecord>&, SkuDetailsRecords);

/**
 *	Query Purchase delegate
 */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FOnQueryPurchasesCompleted,
	EBillingResponseCode, ResponseCode,
	const FString&, ResponseMessage,
	const TArray<FPurchaseRecord>&, PurchaseRecords);

// TODO: add examples
UCLASS()
class UAndroidBillingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 *	Initiates the billing flow for an in-app purchase or subscription.
	 *	
	 *	@param OnOperationResult callback is called when operation has been completed.
	 *	@param ProductID specifies the ID of the item being purchased.
	 *	@param ProductType specifies the Type of the item being purchased.
	 */
	UFUNCTION(BlueprintCallable, Category = "GooglePlayBilling")
	static void LaunchBillingFlow(
		FOnLaunchBillingFlowCompleted OnOperationResult,
		const FString& ProductID,
		const ESkuType ProductType);
	
	/**
	 *	Initiates a flow to confirm the change of price for an item
	 *	subscribed by the user.
	 *
	 *	@param OnOperationResult callback is called when operation has been completed.
	 *	@param ProductID specifies the ID of the item that has the pending price change. 
	 *	@param ProductType specifies the Type of the item that has the pending price change.
	 */
	UFUNCTION(BlueprintCallable, Category = "GooglePlayBilling")
	static void LaunchPriceChangeConfirmationFlow(
		FOnOperationCompleted OnOperationResult,
		const FString& ProductID,
		const ESkuType ProductType);
	
	/**
	 *	Returns purchases details for currently owned items bought
	 *	within your app.
	 *
	 *	@param OnOperationResult callback is called when operation has been completed.
	 *	@param ProductType the type of the product.
	 */
	UFUNCTION(BlueprintCallable, Category = "GooglePlayBilling")
	static void QueryPurchases(
		FOnQueryPurchasesCompleted OnOperationResult,
		const ESkuType ProductType);
	
	/**
	 *	Performs a network query to get SKU details and return the
	 *	result asynchronously.
	 *
	 *	@param OnOperationResult callback is called when operation has been completed.
	 *	@param ProductIDs specifies the ProductIDs that are queried for as
	 *					  published in the Google Developer console.
	 *					  
	 *	@param ProductType specifies the Type of the product to query.
	 */
	UFUNCTION(BlueprintCallable, Category = "GooglePlayBilling")
	static void QuerySkuDetails(
		FOnQuerySkuDetailsCompleted OnOperationResult,
		const TArray<FString>& ProductIDs,
		const ESkuType ProductType);
	
	/**
	 *	Returns the most recent purchase made by the user for each SKU,
	 *	even if that purchase is expired, canceled, or consumed.
	 *
	 *	@param OnOperationResult callback is called when operation has been completed.
	 *	@param ProductType the type of the product.
	 */
	UFUNCTION(BlueprintCallable, Category = "GooglePlayBilling")
	static void QueryPurchaseHistory(
		FOnQueryPurchaseHistoryCompleted OnOperationResult,
		const ESkuType ProductType);
	
	/**
	 *	Checks if the specified feature or capability is supported
	 *	by the Play Store.
	 *
	 *	@param FeatureType one of the EFeatureType constants.
	 */
	UFUNCTION(BlueprintPure, Category = "GooglePlayBilling")
	static EBillingResponseCode IsFeatureSupported(const EFeatureType FeatureType);
	
	/**
	 *	Checks if the client is currently connected to the service, so
	 *	that requests to other methods will succeed.
	 */
	UFUNCTION(BlueprintPure, Category = "GooglePlayBilling")
	static bool IsIAPInitialized();
	
	/**
	 *	Checks if user owning the product.
	 *
	 *	@param ProductID specifies the ID of the item.
	 */
	UFUNCTION(BlueprintPure, Category = "GooglePlayBilling")
	static bool IsPurchased(const FString& ProductID);
};
