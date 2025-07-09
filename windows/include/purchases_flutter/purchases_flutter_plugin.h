#ifndef FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
#define FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter_plugin_registrar.h>

#include <memory>
#include <string>

#if defined(__cplusplus)
extern "C"
{
#endif

    // Function expected by Flutter plugin system
    void PurchasesFlutterPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar);

#if defined(__cplusplus)
} // extern "C"
#endif

// Forward declaration
namespace purchases_flutter
{
    class RevenueCatApi;
}

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
        std::string GenerateRandomUserId();

        // Private member variables
        bool configured_ = false;
        std::string api_key_;
        std::string app_user_id_;
        std::unique_ptr<RevenueCatApi> revenuecat_api_;
    };

} // namespace purchases_flutter

#endif // FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
