#include "include/purchases_flutter/purchases_flutter_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <iostream>

#include "revenuecat_api.h"

namespace purchases_flutter
{

  // static
  void PurchasesFlutterPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "purchases_flutter",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<PurchasesFlutterPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  PurchasesFlutterPlugin::PurchasesFlutterPlugin() {}

  PurchasesFlutterPlugin::~PurchasesFlutterPlugin() {}

  void PurchasesFlutterPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    // Handle setupPurchases method
    if (method_call.method_name().compare("setupPurchases") == 0)
    {
      HandleSetupPurchases(method_call, std::move(result));
    }
    // Handle logIn method
    else if (method_call.method_name().compare("logIn") == 0)
    {
      HandleLogIn(method_call, std::move(result));
    }
    // Handle getAppUserID method
    else if (method_call.method_name().compare("getAppUserID") == 0)
    {
      HandleGetAppUserID(method_call, std::move(result));
    }
    // Handle getPlatformVersion method (existing)
    else if (method_call.method_name().compare("getPlatformVersion") == 0)
    {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater())
      {
        version_stream << "10+";
      }
      else if (IsWindows8OrGreater())
      {
        version_stream << "8";
      }
      else if (IsWindows7OrGreater())
      {
        version_stream << "7";
      }
      result->Success(flutter::EncodableValue(version_stream.str()));
    }
    else
    {
      result->NotImplemented();
    }
  }

  void PurchasesFlutterPlugin::HandleSetupPurchases(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    // Get arguments from method call
    const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (!arguments)
    {
      result->Error("invalid_arguments", "Arguments must be a map");
      return;
    }

    // Extract apiKey and appUserId from arguments
    std::string apiKey = "";
    std::string appUserId = "";

    auto apiKeyIt = arguments->find(flutter::EncodableValue("apiKey"));
    if (apiKeyIt != arguments->end())
    {
      const auto *apiKeyValue = std::get_if<std::string>(&apiKeyIt->second);
      if (apiKeyValue)
      {
        apiKey = *apiKeyValue;
      }
    }

    auto appUserIdIt = arguments->find(flutter::EncodableValue("appUserId"));
    if (appUserIdIt != arguments->end())
    {
      const auto *appUserIdValue = std::get_if<std::string>(&appUserIdIt->second);
      if (appUserIdValue)
      {
        appUserId = *appUserIdValue;
      }
    }

    if (apiKey.empty())
    {
      result->Error("invalid_arguments", "API key is required");
      return;
    }

    // Store configuration
    configured_ = true;
    api_key_ = apiKey;

    // Initialize RevenueCat API client
    revenuecat_api_ = std::make_unique<RevenueCatApi>(apiKey);

    // Generate a random app user ID if not provided
    if (appUserId.empty())
    {
      app_user_id_ = GenerateRandomUserId();
    }
    else
    {
      app_user_id_ = appUserId;
    }

    std::cout << "RevenueCat configured with API key: " << apiKey.substr(0, 8) << "..." << std::endl;
    std::cout << "App User ID: " << app_user_id_ << std::endl;

    // For now, return success - will implement actual HTTP call later
    result->Success();
  }

  void PurchasesFlutterPlugin::HandleLogIn(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (!configured_)
    {
      result->Error("not_configured", "Purchases has not been configured. Call configure() first.");
      return;
    }

    // Get arguments from method call
    const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (!arguments)
    {
      result->Error("invalid_arguments", "Arguments must be a map");
      return;
    }

    // Extract appUserID from arguments
    std::string newAppUserId = "";
    auto appUserIdIt = arguments->find(flutter::EncodableValue("appUserID"));
    if (appUserIdIt != arguments->end())
    {
      const auto *appUserIdValue = std::get_if<std::string>(&appUserIdIt->second);
      if (appUserIdValue)
      {
        newAppUserId = *appUserIdValue;
      }
    }

    if (newAppUserId.empty())
    {
      result->Error("invalid_arguments", "appUserID cannot be empty");
      return;
    }

    if (!revenuecat_api_)
    {
      result->Error("not_configured", "RevenueCat API not initialized");
      return;
    }

    // Store the previous user ID to determine if this is a new user
    app_user_id_ = newAppUserId;

    std::cout << "Logging in user: " << newAppUserId << std::endl;

    // Make actual API call to RevenueCat
    auto loginResult = revenuecat_api_->LogIn(newAppUserId);

    if (loginResult.success)
    {
      // Create LogInResult
      flutter::EncodableMap logInResult;
      logInResult[flutter::EncodableValue("customerInfo")] = flutter::EncodableValue(loginResult.customerInfo);
      logInResult[flutter::EncodableValue("created")] = flutter::EncodableValue(loginResult.created);

      std::cout << "Login successful. Created: " << (loginResult.created ? "true" : "false") << std::endl;
      result->Success(flutter::EncodableValue(logInResult));
    }
    else
    {
      std::cout << "Login failed: " << loginResult.error << std::endl;
      result->Error("login_failed", loginResult.error);
    }
  }

  void PurchasesFlutterPlugin::HandleGetAppUserID(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (!configured_)
    {
      result->Error("not_configured", "Purchases has not been configured. Call configure() first.");
      return;
    }

    result->Success(flutter::EncodableValue(app_user_id_));
  }

  flutter::EncodableMap PurchasesFlutterPlugin::CreateEmptyCustomerInfo()
  {
    flutter::EncodableMap customerInfo;

    // Create empty entitlements
    flutter::EncodableMap entitlements;
    entitlements[flutter::EncodableValue("all")] = flutter::EncodableValue(flutter::EncodableMap());
    entitlements[flutter::EncodableValue("active")] = flutter::EncodableValue(flutter::EncodableMap());
    customerInfo[flutter::EncodableValue("entitlements")] = flutter::EncodableValue(entitlements);

    // Empty purchase dates and other fields
    customerInfo[flutter::EncodableValue("allPurchaseDates")] = flutter::EncodableValue(flutter::EncodableMap());
    customerInfo[flutter::EncodableValue("activeSubscriptions")] = flutter::EncodableValue(flutter::EncodableList());
    customerInfo[flutter::EncodableValue("allPurchasedProductIdentifiers")] = flutter::EncodableValue(flutter::EncodableList());
    customerInfo[flutter::EncodableValue("nonSubscriptionTransactions")] = flutter::EncodableValue(flutter::EncodableList());
    customerInfo[flutter::EncodableValue("allExpirationDates")] = flutter::EncodableValue(flutter::EncodableMap());

    // Set dates to current time
    std::time_t now = std::time(nullptr);
    std::ostringstream dateStream;

    struct tm timeinfo;
    gmtime_s(&timeinfo, &now);
    dateStream << std::put_time(&timeinfo, "%Y-%m-%dT%H:%M:%SZ");
    std::string currentDate = dateStream.str();

    customerInfo[flutter::EncodableValue("firstSeen")] = flutter::EncodableValue(currentDate);
    customerInfo[flutter::EncodableValue("requestDate")] = flutter::EncodableValue(currentDate);
    customerInfo[flutter::EncodableValue("originalAppUserId")] = flutter::EncodableValue(app_user_id_);

    return customerInfo;
  }

  std::string PurchasesFlutterPlugin::GenerateRandomUserId()
  {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.reserve(32);

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < 32; ++i)
    {
      result += chars[rand() % (sizeof(chars) - 1)];
    }

    return "$RCAnonymousID:" + result;
  }

  // Implementation of the function expected by Flutter plugin system
  void PurchasesFlutterPluginRegisterWithRegistrar(
      FlutterDesktopPluginRegistrarRef registrar)
  {
    PurchasesFlutterPlugin::RegisterWithRegistrar(
        flutter::PluginRegistrarManager::GetInstance()
            ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
  }

} // namespace purchases_flutter
