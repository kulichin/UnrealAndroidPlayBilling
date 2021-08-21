// Copyright (C) 2021. Nikita Klimov. All rights reserved.

package com.kulichin.googleplaybilling;

import android.app.Activity;

import androidx.annotation.NonNull;

import com.android.billingclient.api.AccountIdentifiers;
import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.PriceChangeFlowParams;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchaseHistoryRecord;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

public class GooglePlayBilling
{
    public static native void NativeCompleteOperation(int ResponseCode, String ResponseMessage);
    public static native void NativeCompleteLaunchBillingFlow(
        int ResponseCode,
        String ResponseMessage,
        PurchaseRecord[] Array,
        int ArraySize);

    public static native void NativeCompleteQueryPurchaseHistory(
        int ResponseCode,
        String ResponseMessage,
        PurchaseRecordFromHistory[] Array,
		int ArraySize);

    public static native void NativeCompleteQuerySkuDetails(
        int ResponseCode,
        String ResponseMessage,
        SkuDetailsRecord[] Array,
		int ArraySize);

    public static native void NativeCompleteQueryPurchases(
        int ResponseCode,
        String ResponseMessage,
        PurchaseRecord[] Array,
		int ArraySize);

	public static native void NativeInitialize(
		Class SkuDetailsRecordClass,
		Class PurchaseRecordFromHistoryClass,
		Class PurchaseRecordClass
	);

    public static class SkuDetailsRecord
    {
        public String OriginalJSON;
        public String ProductID;
        public String Type;
        public String Price;
        public long PriceAmountMicros;
        public String PriceCurrencyCode;
        public String OriginalPrice;
        public long OriginalPriceAmountMicros;
        public String Title;
        public String Description;
        public String SubscriptionPeriod;
        public String FreeTrialPeriod;
        public String IntroductoryPrice;
        public long IntroductoryPriceAmountMicros;
        public String IntroductoryPricePeriod;
        public int IntroductoryPriceCycles;
        public String IconURL;

        SkuDetailsRecord(
            String OriginalJSON,
            String ProductID,
            String Type,
            String Price,
            long PriceAmountMicros,
            String PriceCurrencyCode,
            String OriginalPrice,
            long OriginalPriceAmountMicros,
            String Title,
            String Description,
            String SubscriptionPeriod,
            String FreeTrialPeriod,
            String IntroductoryPrice,
            long IntroductoryPriceAmountMicros,
            String IntroductoryPricePeriod,
            int IntroductoryPriceCycles,
            String IconURL
        )
        {
            this.OriginalJSON = OriginalJSON;
            this.ProductID = ProductID;
            this.Type = Type;
            this.Price = Price;
            this.PriceAmountMicros = PriceAmountMicros;
            this.PriceCurrencyCode = PriceCurrencyCode;
            this.OriginalPrice = OriginalPrice;
            this.OriginalPriceAmountMicros = OriginalPriceAmountMicros;
            this.Title = Title;
            this.Description = Description;
            this.SubscriptionPeriod = SubscriptionPeriod;
            this.FreeTrialPeriod = FreeTrialPeriod;
            this.IntroductoryPrice = IntroductoryPrice;
            this.IntroductoryPriceAmountMicros = IntroductoryPriceAmountMicros;
            this.IntroductoryPricePeriod = IntroductoryPricePeriod;
            this.IntroductoryPriceCycles = IntroductoryPriceCycles;
            this.IconURL = IconURL;
        }
    }

    public static class PurchaseRecordFromHistory
    {
        public String ProductID;
        public long PurchaseTime;
        public String PurchaseToken;
        public String DeveloperPayload;
        public String OriginalJSON;
        public String Signature;

        PurchaseRecordFromHistory(
            String ProductID,
            long PurchaseTime,
            String PurchaseToken,
            String DeveloperPayload,
            String OriginalJSON,
            String Signature
        )
        {
            this.ProductID = ProductID;
            this.PurchaseTime = PurchaseTime;
            this.PurchaseToken = PurchaseToken;
            this.DeveloperPayload = DeveloperPayload;
            this.OriginalJSON = OriginalJSON;
            this.Signature = Signature;
        }
    }

    public static class PurchaseRecord
    {
        public String OrderID;
        public String PackageName;
        public String ProductID;
        public long PurchaseTime;
        public String PurchaseToken;
        public int PurchaseState;
        public String DeveloperPayload;
        public boolean bAcknowledged;
        public boolean bAutoRenewing;
        public String OriginalJSON;
        public String Signature;
        public String ObfuscatedAccountID;
        public String ObfuscatedProfileID;

        PurchaseRecord(
            String OrderID,
            String PackageName,
            String ProductID,
            long PurchaseTime,
            String PurchaseToken,
            int PurchaseState,
            String DeveloperPayload,
            boolean bAcknowledged,
            boolean bAutoRenewing,
            String OriginalJSON,
            String Signature,
            String ObfuscatedAccountID,
            String ObfuscatedProfileID)
        {
            this.OrderID = OrderID;
            this.PackageName = PackageName;
            this.ProductID = ProductID;
            this.PurchaseTime = PurchaseTime;
            this.PurchaseToken = PurchaseToken;
            this.PurchaseState = PurchaseState;
            this.DeveloperPayload = DeveloperPayload;
            this.bAcknowledged = bAcknowledged;
            this.bAutoRenewing = bAutoRenewing;
            this.OriginalJSON = OriginalJSON;
            this.Signature = Signature;
            this.ObfuscatedAccountID = ObfuscatedAccountID;
            this.ObfuscatedProfileID = ObfuscatedProfileID;
        }
    }

	private final int PRODUCT_INAPP_ID = 0;
    private final int PRODUCT_SUBS_ID = 1;
    private final String[] ProductTypes =
    {
        "inapp",
        "subs"
    };

    private final String[] FeatureTypes =
    {
        "inAppItemsOnVr",
        "priceChangeConfirmation",
        "subscriptions",
        "subscriptionsOnVr",
        "subscriptionsUpdate"
    };

	private final Activity GameActivity;
    private final HashMap<String, PurchaseRecord> PurchaseRecords = new HashMap<>();
    private final BillingClient AppBillingClient;

	public GooglePlayBilling(Activity GameActivity)
	{
		this.GameActivity = GameActivity;

		final PurchasesUpdatedListener PurchasesListener = (Result, Purchases) ->
		{
			PurchaseRecord[] WrappedPurchaseRecords = null;
			int WrappedPurchaseRecordsSize = 0;
		    if (Purchases != null)
		    {
				WrappedPurchaseRecords = new PurchaseRecord[Purchases.size()];
				WrappedPurchaseRecordsSize = Purchases.size();

		        for (int Idx = 0; Idx < Purchases.size(); Idx++)
		        {
					// Wrap to our class
					Purchase PurchaseRecord = Purchases.get(Idx);
					PurchaseRecord WrappedPurchaseRecord = WrapToPurchaseRecord(PurchaseRecord);

		            // Put record to hash map
		            String ProductID = WrappedPurchaseRecord.ProductID;
		            PurchaseRecords.put(ProductID, WrappedPurchaseRecord);

					// Append to 'WrappedPurchaseRecords'
					WrappedPurchaseRecords[Idx] = WrappedPurchaseRecord;

					// Handle purchase
					HandlePurchaseInternal(PurchaseRecord);
		        }
		    }

			// Complete operation
            NativeCompleteLaunchBillingFlow(
                Result.getResponseCode(),
                Result.getDebugMessage(),
                WrappedPurchaseRecords,
                WrappedPurchaseRecordsSize);
		};

		// Initialize billing client
		AppBillingClient = BillingClient.newBuilder(GameActivity)
            .setListener(PurchasesListener)
            .enablePendingPurchases()
            .build();

		AppBillingClient.startConnection(new BillingClientStateListener()
        {
            @Override
            public void onBillingSetupFinished(@NonNull BillingResult Result)
            {
				QueryBillingDetailsInternal();
            }

            // TODO: try to restart google billing
            @Override
            public void onBillingServiceDisconnected()
            {
                // Try to restart the connection on the next request to
                // Google Play by calling the startConnection() method.
            }
        });

		// Call native initializator
		NativeInitialize(
			SkuDetailsRecord.class,
			PurchaseRecordFromHistory.class,
			PurchaseRecord.class);
	}

	public void OnDestroy()
	{
		AppBillingClient.endConnection();
	}

	public void LaunchBillingFlow(String ProductID, int ProductType)
    {
		// Query Sku details & start purchase
        QuerySkuDetailsInternal(new String[] { ProductID }, ProductType, (QueryResult, SkuDetailsList) ->
        {
			SkuDetails Details = ConstructSkuDetailsInternal(ProductID, ProductType);
            if (SkuDetailsList != null && SkuDetailsList.size() > 0)
			{
				Details = SkuDetailsList.get(0);
			}

			// Build BillingFlowParams
			BillingFlowParams FlowParams = BillingFlowParams.newBuilder()
				.setSkuDetails(Details)
			    .build();

			BillingResult FlowResult = AppBillingClient.launchBillingFlow(GameActivity, FlowParams);
			if (FlowResult.getResponseCode() != BillingClient.BillingResponseCode.OK)
			{
				// Complete operation
			    NativeCompleteLaunchBillingFlow(
			        FlowResult.getResponseCode(),
			        FlowResult.getDebugMessage(),
			        null,
					0);
			}
			// else: wait for 'PurchasesListener' callback
		});
    }

    public void LaunchPriceChangeConfirmationFlow(String ProductID, int ProductType)
    {
		// Query Sku details & start purchase
		QuerySkuDetailsInternal(new String[] { ProductID }, ProductType, (QueryResult, SkuDetailsList) ->
        {
			SkuDetails Details = ConstructSkuDetailsInternal(ProductID, ProductType);
            if (SkuDetailsList != null && SkuDetailsList.size() > 0)
			{
				Details = SkuDetailsList.get(0);
			}

			// Build PriceChangeParams
			PriceChangeFlowParams PriceChangeParams = PriceChangeFlowParams.newBuilder()
				.setSkuDetails(Details)
				.build();
	
			AppBillingClient.launchPriceChangeConfirmationFlow(
			    GameActivity,
			    PriceChangeParams,
			    FlowResult ->
					// Complete operation
			        NativeCompleteOperation(
			            FlowResult.getResponseCode(),
			            FlowResult.getDebugMessage()));
		});
	}

    public void QueryPurchases(int ProductType)
    {
        Purchase.PurchasesResult Result = QueryPurchasesInternal(ProductType);
        List<Purchase> Purchases = Result.getPurchasesList();

        if (Result.getResponseCode() == BillingClient.BillingResponseCode.OK &&
            Purchases != null)
        {
            // Construct array for wrapped records
            int PurchasesSize = Purchases.size();
            PurchaseRecord[] Records = new PurchaseRecord[PurchasesSize];
            for (int Idx = 0; Idx < PurchasesSize; Idx++)
            {
                Records[Idx] = WrapToPurchaseRecord(Purchases.get(Idx));
            }

            // Complete operation
            NativeCompleteQueryPurchases(
                Result.getResponseCode(),
                Result.getBillingResult().getDebugMessage(),
                Records,
				Records.length);
        }
        else
        {
            // Complete operation
            NativeCompleteQueryPurchases(
                Result.getResponseCode(),
                Result.getBillingResult().getDebugMessage(),
                null,
				0);
        }
    }

    public void QuerySkuDetails(String[] ProductIDs, int ProductType)
    {
        // Query Sku details & start purchase
		QuerySkuDetailsInternal(ProductIDs, ProductType, (Result, SkuDetailsList) ->
        {
            if (Result.getResponseCode() == BillingClient.BillingResponseCode.OK &&
                SkuDetailsList != null)
            {
                final int SkuDetailsSize = SkuDetailsList.size();
                SkuDetailsRecord[] DetailsRecords = new SkuDetailsRecord[SkuDetailsSize];

                for (int Idx = 0; Idx < SkuDetailsSize; Idx++)
                {
                    SkuDetails Details = SkuDetailsList.get(Idx);
                    DetailsRecords[Idx] = WrapToSkuDetailsRecord(Details);
                }

                // Complete operation
                NativeCompleteQuerySkuDetails(
                    Result.getResponseCode(),
                    Result.getDebugMessage(),
                    DetailsRecords,
					DetailsRecords.length);
            }
            else
            {
                // Complete operation
                NativeCompleteQuerySkuDetails(
                    Result.getResponseCode(),
                    Result.getDebugMessage(),
                    null,
					0);
            }
        });
    }

    public void QueryPurchaseHistory(int ProductType)
    {
        AppBillingClient.queryPurchaseHistoryAsync(ProductTypes[ProductType], (Result, Purchases) ->
        {
            if (Result.getResponseCode() == BillingClient.BillingResponseCode.OK &&
                Purchases != null)
            {
                // Construct array for wrapped records
                int PurchasesSize = Purchases.size();
                PurchaseRecordFromHistory[] Records = new PurchaseRecordFromHistory[PurchasesSize];
                for (int Idx = 0; Idx < PurchasesSize; Idx++)
                {
                    Records[Idx] = WrapToPurchaseRecordFromHistory(Purchases.get(Idx));
                }

                // Complete operation
                NativeCompleteQueryPurchaseHistory(
                    Result.getResponseCode(),
                    Result.getDebugMessage(),
                    Records,
					Records.length);
            }
            else
            {
                // Complete operation
                NativeCompleteQueryPurchaseHistory(
                    Result.getResponseCode(),
                    Result.getDebugMessage(),
                    null,
					0);
            }
        });
    }

    public int IsFeatureSupported(int FeatureType)
    {
        BillingResult Result = AppBillingClient.isFeatureSupported(FeatureTypes[FeatureType]);
        return Result.getResponseCode();
    }

    public boolean IsIAPInitialized()
    {
        if (AppBillingClient != null)
        {
            return AppBillingClient.isReady();
        }

        return false;
    }

    public boolean IsPurchased(String ProductID)
    {
        return PurchaseRecords.containsKey(ProductID);
    }

	// Internal
	private Purchase.PurchasesResult QueryPurchasesInternal(int ProductType)
    {
        return AppBillingClient.queryPurchases(ProductTypes[ProductType]);
    }

	private void QueryBillingDetailsInternal()
	{
		// Query purchases on resume
        Purchase.PurchasesResult SubscriptionsResult = QueryPurchasesInternal(PRODUCT_SUBS_ID);
        Purchase.PurchasesResult ProductsResult = QueryPurchasesInternal(PRODUCT_INAPP_ID);

		// Handle purchases
		for (Purchase Subscription : SubscriptionsResult.getPurchasesList())
		{
			HandlePurchaseInternal(Subscription);
		}

		for (Purchase Product : ProductsResult.getPurchasesList())
		{
			HandlePurchaseInternal(Product);
		}
	}

    private SkuDetails ConstructSkuDetailsInternal(String ProductID, int ProductType)
	{
		try
		{
		    String SkuDetailsJSON = new JSONObject()
				.put("productId", ProductID)
				.put("type", ProductTypes[ProductType])
				.toString();

		    return new SkuDetails(SkuDetailsJSON);
		}
		catch (JSONException ignored)
		{
			return null;
		}
	}

	// Internal
    private void QuerySkuDetailsInternal(
        String[] ProductIDs,
        int ProductType,
        SkuDetailsResponseListener SkuDetailsListener)
    {
		// Build Sku details
        List<String> SkuList = Arrays.asList(ProductIDs);
        SkuDetailsParams SkuDetails = SkuDetailsParams.newBuilder()
            .setType(ProductTypes[ProductType])
            .setSkusList(SkuList)
            .build();

        // Query Sku details & start purchase
        AppBillingClient.querySkuDetailsAsync(SkuDetails, SkuDetailsListener);
    }

	public void HandlePurchaseInternal(Purchase PurchaseRecord)
    {
        if (PurchaseRecord != null && PurchaseRecord.getDeveloperPayload().isEmpty())
        {
			// Try to consume item
			ConsumeParams Params = ConsumeParams.newBuilder()
			    .setPurchaseToken(PurchaseRecord.getPurchaseToken())
			    .build();

			AppBillingClient.consumeAsync(Params, (ConsumeResult, PurchaseToken) ->
			{
				// If item not consumed then acknowledge them
				if (ConsumeResult.getResponseCode() != BillingClient.BillingResponseCode.OK)
				{
					AcknowledgePurchaseParams AcknowledgeParams = AcknowledgePurchaseParams.newBuilder()
						.setPurchaseToken(PurchaseRecord.getPurchaseToken())
						.build();

					AppBillingClient.acknowledgePurchase(AcknowledgeParams, AcknowledgeResult -> {});
				}
			});
		}
    }

	// Wrappers
	private PurchaseRecord WrapToPurchaseRecord(Purchase Record)
    {
        // Check validity of 'getAccountIdentifiers'
        String ObfuscatedAccountID = "";
        String ObfuscatedProfileID = "";
        AccountIdentifiers Identifiers = Record.getAccountIdentifiers();
        if (Identifiers != null)
        {
            ObfuscatedAccountID = Identifiers.getObfuscatedAccountId();
            ObfuscatedProfileID = Identifiers.getObfuscatedProfileId();
        }

        return new PurchaseRecord
        (
            Record.getOrderId(),
            Record.getPackageName(),
            Record.getSkus().get(0),
            Record.getPurchaseTime(),
            Record.getPurchaseToken(),
            Record.getPurchaseState(),
            Record.getDeveloperPayload(),
            Record.isAcknowledged(),
            Record.isAutoRenewing(),
            Record.getOriginalJson(),
            Record.getSignature(),
            ObfuscatedAccountID,
            ObfuscatedProfileID
        );
    }

    private PurchaseRecordFromHistory WrapToPurchaseRecordFromHistory(PurchaseHistoryRecord Record)
    {
        return new PurchaseRecordFromHistory
        (
            Record.getSkus().get(0),
            Record.getPurchaseTime(),
            Record.getPurchaseToken(),
            Record.getDeveloperPayload(),
            Record.getOriginalJson(),
            Record.getSignature()
        );
    }

    private SkuDetailsRecord WrapToSkuDetailsRecord(SkuDetails Record)
    {
        return new SkuDetailsRecord(
            Record.getOriginalJson(),
            Record.getSku(),
            Record.getType(),
            Record.getPrice(),
            Record.getPriceAmountMicros(),
            Record.getPriceCurrencyCode(),
            Record.getOriginalPrice(),
            Record.getOriginalPriceAmountMicros(),
            Record.getTitle(),
            Record.getDescription(),
            Record.getSubscriptionPeriod(),
            Record.getFreeTrialPeriod(),
            Record.getIntroductoryPrice(),
            Record.getIntroductoryPriceAmountMicros(),
            Record.getIntroductoryPricePeriod(),
            Record.getIntroductoryPriceCycles(),
            Record.getIconUrl()
        );
    }
}
