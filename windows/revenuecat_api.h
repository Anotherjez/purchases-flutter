#pragma once

#include <string>
#include <flutter/encodable_value.h>
#include "http_client.h"

namespace purchases_flutter
{

    class RevenueCatApi
    {
    public:
        RevenueCatApi(const std::string &apiKey);
        ~RevenueCatApi();

        // Login user - creates or updates subscriber
        struct LogInResult
        {
            bool success;
            bool created;
            flutter::EncodableMap customerInfo;
            std::string error;
        };

        LogInResult LogIn(const std::string &appUserId);
        flutter::EncodableMap GetCustomerInfo(const std::string &appUserId);

    private:
        std::string api_key_;
        HttpClient http_client_;

        std::string GetCurrentISODate();
        std::map<std::string, std::string> CreateAuthHeaders();
        flutter::EncodableMap CreateCustomerInfoFromJson(const std::string &json);
        flutter::EncodableMap CreateEmptyCustomerInfo(const std::string &appUserId);
    };

} // namespace purchases_flutter
