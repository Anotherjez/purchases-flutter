#include "include/purchases_flutter/purchases_flutter_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>
#include <winhttp.h>

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
#include <thread>
#include <functional>

#pragma comment(lib, "winhttp.lib")

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

        // Generate a random app user ID if not provided
        if (appUserId.empty())
        {
            app_user_id_ = GenerateRandomUserId();
        }
        else
        {
            app_user_id_ = appUserId;
        }

        // For now, just return success - we'll implement API call later
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

        // Store the previous user ID to determine if this is a new user
        bool created = (app_user_id_ != newAppUserId);
        app_user_id_ = newAppUserId;

        // Make actual API call to RevenueCat to create/update subscriber
        // This is what RevenueCat's native SDKs do internally
        std::string endpoint = "/subscribers/" + newAppUserId;

        // Create JSON payload for subscriber update
        std::string jsonPayload = "{\"app_user_id\":\"" + newAppUserId + "\",\"last_seen\":\"" + GetCurrentISODate() + "\"}";

        // Create a shared pointer to the result for the async callback
        auto resultPtr = std::make_shared<std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>>(std::move(result));

        MakeHttpRequest("POST", endpoint, jsonPayload, [this, resultPtr, created](bool success, const std::string &response)
                        {
      if (success) {
        // Parse the response and create CustomerInfo
        flutter::EncodableMap customerInfo = CreateCustomerInfoFromResponse(response);
        
        // Create LogInResult
        flutter::EncodableMap logInResult;
        logInResult[flutter::EncodableValue("customerInfo")] = flutter::EncodableValue(customerInfo);
        logInResult[flutter::EncodableValue("created")] = flutter::EncodableValue(created);

        (*resultPtr)->Success(flutter::EncodableValue(logInResult));
      } else {
        // Fallback to simulated response if API call fails
        flutter::EncodableMap customerInfo = CreateEmptyCustomerInfo();
        
        flutter::EncodableMap logInResult;
        logInResult[flutter::EncodableValue("customerInfo")] = flutter::EncodableValue(customerInfo);
        logInResult[flutter::EncodableValue("created")] = flutter::EncodableValue(created);

        (*resultPtr)->Success(flutter::EncodableValue(logInResult));
      } });
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

    flutter::EncodableMap PurchasesFlutterPlugin::CreateCustomerInfoFromResponse(const std::string &response)
    {
        // For now, we'll do basic JSON parsing manually
        flutter::EncodableMap customerInfo = CreateEmptyCustomerInfo();

        // Try to extract some basic information from the response
        if (response.find("\"subscriber\"") != std::string::npos)
        {
            // Parse basic subscriber info
            size_t userIdPos = response.find("\"original_app_user_id\"");
            if (userIdPos != std::string::npos)
            {
                size_t valueStart = response.find("\"", userIdPos + 22);
                size_t valueEnd = response.find("\"", valueStart + 1);
                if (valueStart != std::string::npos && valueEnd != std::string::npos)
                {
                    std::string extractedUserId = response.substr(valueStart + 1, valueEnd - valueStart - 1);
                    customerInfo[flutter::EncodableValue("originalAppUserId")] = flutter::EncodableValue(extractedUserId);
                }
            }

            // Extract first seen date
            size_t firstSeenPos = response.find("\"first_seen\"");
            if (firstSeenPos != std::string::npos)
            {
                size_t valueStart = response.find("\"", firstSeenPos + 12);
                size_t valueEnd = response.find("\"", valueStart + 1);
                if (valueStart != std::string::npos && valueEnd != std::string::npos)
                {
                    std::string firstSeenDate = response.substr(valueStart + 1, valueEnd - valueStart - 1);
                    customerInfo[flutter::EncodableValue("firstSeen")] = flutter::EncodableValue(firstSeenDate);
                }
            }
        }

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

    std::string PurchasesFlutterPlugin::GetRevenueCatApiUrl() const
    {
        return "https://api.revenuecat.com/v1/";
    }

    std::string PurchasesFlutterPlugin::PrepareAuthHeaders() const
    {
        return "Authorization: Bearer " + api_key_;
    }

    void PurchasesFlutterPlugin::MakeHttpRequest(
        const std::string &method,
        const std::string &endpoint,
        const std::string &body,
        std::function<void(bool success, const std::string &response)> callback)
    {
        std::thread([this, method, endpoint, body, callback]()
                    {
      HINTERNET hSession = nullptr;
      HINTERNET hConnect = nullptr;
      HINTERNET hRequest = nullptr;
      
      try {
        // Initialize WinHTTP
        hSession = WinHttpOpen(L"RevenueCat-Windows-SDK/1.0",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME,
                              WINHTTP_NO_PROXY_BYPASS,
                              0);
        
        if (!hSession) {
          callback(false, "Failed to initialize HTTP session");
          return;
        }

        // Connect to server
        hConnect = WinHttpConnect(hSession, L"api.revenuecat.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
          callback(false, "Failed to connect to RevenueCat API");
          return;
        }

        // Convert endpoint to wide string
        std::wstring wEndpoint(endpoint.begin(), endpoint.end());
        std::wstring wMethod(method.begin(), method.end());

        // Create request
        hRequest = WinHttpOpenRequest(hConnect,
                                     wMethod.c_str(),
                                     wEndpoint.c_str(),
                                     nullptr,
                                     WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     WINHTTP_FLAG_SECURE);

        if (!hRequest) {
          callback(false, "Failed to create HTTP request");
          return;
        }

        // Set headers
        std::string headers = "Content-Type: application/json\r\n";
        headers += PrepareAuthHeaders() + "\r\n";
        
        std::wstring wHeaders(headers.begin(), headers.end());
        WinHttpAddRequestHeaders(hRequest, wHeaders.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);

        // Send request
        BOOL bResult = WinHttpSendRequest(hRequest,
                                         WINHTTP_NO_ADDITIONAL_HEADERS,
                                         0,
                                         body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.c_str(),
                                         static_cast<DWORD>(body.length()),
                                         static_cast<DWORD>(body.length()),
                                         0);

        if (!bResult) {
          callback(false, "Failed to send HTTP request");
          return;
        }

        // Wait for response
        bResult = WinHttpReceiveResponse(hRequest, nullptr);
        if (!bResult) {
          callback(false, "Failed to receive HTTP response");
          return;
        }

        // Read response
        std::string response;
        DWORD dwSize = 0;
        do {
          dwSize = 0;
          if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            callback(false, "Failed to query data availability");
            return;
          }

          if (dwSize == 0) break;

          std::vector<char> buffer(dwSize + 1);
          DWORD dwDownloaded = 0;
          if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
            callback(false, "Failed to read response data");
            return;
          }

          buffer[dwDownloaded] = '\0';
          response += buffer.data();
        } while (dwSize > 0);

        callback(true, response);

      } catch (...) {
        callback(false, "Exception occurred during HTTP request");
      }

      // Cleanup
      if (hRequest) WinHttpCloseHandle(hRequest);
      if (hConnect) WinHttpCloseHandle(hConnect);
      if (hSession) WinHttpCloseHandle(hSession); })
            .detach();
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
