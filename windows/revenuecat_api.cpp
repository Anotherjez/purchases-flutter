#include "revenuecat_api.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace purchases_flutter
{

    RevenueCatApi::RevenueCatApi(const std::string &apiKey) : api_key_(apiKey), http_client_()
    {
    }

    RevenueCatApi::~RevenueCatApi()
    {
    }

    std::string RevenueCatApi::GetCurrentISODate()
    {
        std::time_t now = std::time(nullptr);
        std::ostringstream dateStream;

        struct tm timeinfo;
        gmtime_s(&timeinfo, &now);
        dateStream << std::put_time(&timeinfo, "%Y-%m-%dT%H:%M:%SZ");

        return dateStream.str();
    }

    std::map<std::string, std::string> RevenueCatApi::CreateAuthHeaders()
    {
        std::map<std::string, std::string> headers;
        headers["Authorization"] = "Bearer " + api_key_;
        headers["Content-Type"] = "application/json";
        headers["X-Platform"] = "windows";
        headers["X-Version"] = "1.0.0";
        return headers;
    }

    RevenueCatApi::LogInResult RevenueCatApi::LogIn(const std::string &appUserId)
    {
        LogInResult result;
        result.success = false;
        result.created = false;

        try
        {
            // First, try to get existing subscriber
            std::string getUrl = "https://api.revenuecat.com/v1/subscribers/" + appUserId;
            auto getResponse = http_client_.Get(getUrl, CreateAuthHeaders());

            bool userExists = (getResponse.statusCode == 200);
            result.created = !userExists;

            if (userExists)
            {
                // User exists, parse the response
                std::cout << "User exists. Response: " << getResponse.body << std::endl;
                result.customerInfo = CreateCustomerInfoFromJson(getResponse.body);
            }
            else
            {
                // User doesn't exist, create them by making a POST request
                std::string postUrl = "https://api.revenuecat.com/v1/subscribers/" + appUserId;
                std::string postBody = R"({
                    "app_user_id": ")" +
                                       appUserId + R"(",
                    "last_seen": ")" + GetCurrentISODate() +
                                       R"("
                })";

                auto postResponse = http_client_.Post(postUrl, postBody, CreateAuthHeaders());

                std::cout << "Creating user. Status: " << postResponse.statusCode << ", Response: " << postResponse.body << std::endl;

                if (postResponse.statusCode == 200 || postResponse.statusCode == 201)
                {
                    result.customerInfo = CreateCustomerInfoFromJson(postResponse.body);
                }
                else
                {
                    // If POST fails, create empty customer info
                    result.customerInfo = CreateEmptyCustomerInfo(appUserId);
                }
            }

            result.success = true;
        }
        catch (const std::exception &e)
        {
            result.error = "Exception: " + std::string(e.what());
            result.customerInfo = CreateEmptyCustomerInfo(appUserId);
            std::cout << "Exception in LogIn: " << result.error << std::endl;
        }

        return result;
    }

    flutter::EncodableMap RevenueCatApi::GetCustomerInfo(const std::string &appUserId)
    {
        try
        {
            std::string url = "https://api.revenuecat.com/v1/subscribers/" + appUserId;
            auto response = http_client_.Get(url, CreateAuthHeaders());

            if (response.statusCode == 200)
            {
                return CreateCustomerInfoFromJson(response.body);
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Exception in GetCustomerInfo: " << e.what() << std::endl;
        }

        return CreateEmptyCustomerInfo(appUserId);
    }

    flutter::EncodableMap RevenueCatApi::CreateCustomerInfoFromJson(const std::string &json)
    {
        // For now, create a basic customer info structure
        // In a real implementation, you'd parse the JSON response
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

        std::string currentDate = GetCurrentISODate();
        customerInfo[flutter::EncodableValue("firstSeen")] = flutter::EncodableValue(currentDate);
        customerInfo[flutter::EncodableValue("requestDate")] = flutter::EncodableValue(currentDate);

        // Try to extract originalAppUserId from JSON if possible
        // For now, just set it to empty, but in a real implementation you'd parse the JSON
        customerInfo[flutter::EncodableValue("originalAppUserId")] = flutter::EncodableValue("");

        return customerInfo;
    }

    flutter::EncodableMap RevenueCatApi::CreateEmptyCustomerInfo(const std::string &appUserId)
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

        std::string currentDate = GetCurrentISODate();
        customerInfo[flutter::EncodableValue("firstSeen")] = flutter::EncodableValue(currentDate);
        customerInfo[flutter::EncodableValue("requestDate")] = flutter::EncodableValue(currentDate);
        customerInfo[flutter::EncodableValue("originalAppUserId")] = flutter::EncodableValue(appUserId);

        return customerInfo;
    }

} // namespace purchases_flutter
