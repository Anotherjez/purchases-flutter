#include "include/purchases_flutter/purchases_flutter_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "include/purchases_flutter/purchases_flutter_plugin.h"

void PurchasesFlutterPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  purchases_flutter::PurchasesFlutterPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
