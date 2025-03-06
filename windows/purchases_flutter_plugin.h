#ifndef FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
#define FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace purchases_flutter {

class PurchasesFlutterPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  PurchasesFlutterPlugin();

  virtual ~PurchasesFlutterPlugin();

  // Disallow copy and assign.
  PurchasesFlutterPlugin(const PurchasesFlutterPlugin&) = delete;
  PurchasesFlutterPlugin& operator=(const PurchasesFlutterPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace purchases_flutter

#endif  // FLUTTER_PLUGIN_PURCHASES_FLUTTER_PLUGIN_H_
