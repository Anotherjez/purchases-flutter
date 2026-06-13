#ifndef FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
#define FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <string>
#include <thread>
#include <functional>

namespace purchases_flutter
{

    class PurchasesFlutterPlugin : public flutter::Plugin
    {
    public:
        static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

        PurchasesFlutterPlugin();

        virtual ~PurchasesFlutterPlugin();

        // Disallow copy and assign.
        PurchasesFlutterPlugin(const PurchasesFlutterPlugin &) = delete;
        PurchasesFlutterPlugin &operator=(const PurchasesFlutterPlugin &) = delete;

        // Called when a method is called on this plugin's channel from Dart.
        void HandleMethodCall(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    private:
        // Method handlers
        void HandleSetupPurchases(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

        void HandleLogIn(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

        void HandleGetAppUserID(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

        // Utility methods
        flutter::EncodableMap CreateEmptyCustomerInfo();
        flutter::EncodableMap CreateCustomerInfoFromResponse(const std::string &response);
        std::string GenerateRandomUserId();

        // HTTP methods for RevenueCat API
        void CallRevenueCatAPI(const std::string &endpoint, const std::string &method, const std::string &data, std::function<void(bool, const std::string &)> callback);

        // Private member variables
        bool configured_ = false;
        std::string api_key_;
        std::string app_user_id_;
    };

} // namespace purchases_flutter

#endif // FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
