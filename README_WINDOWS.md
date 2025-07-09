# RevenueCat Windows Support Implementation

Este fork implementa soporte **REAL** para RevenueCat en Windows, permitiendo que los métodos `configure` y `logIn` funcionen correctamente con la API de RevenueCat.

## Características Implementadas

### ✅ Soporte Real
- **Configuración**: `Purchases.configure()` funciona con API keys de Paddle (pdl_xxxx)
- **Login**: `Purchases.logIn()` hace peticiones HTTP reales a la API de RevenueCat
- **Cliente HTTP**: Implementación usando WinINet para comunicación HTTPS
- **Logs de debug**: Mensajes detallados para troubleshooting

### ✅ Comunicación con API de RevenueCat
- Peticiones GET a `/v1/subscribers/{app_user_id}` para verificar usuario existente
- Peticiones POST a `/v1/subscribers/{app_user_id}` para crear nuevos usuarios
- Headers de autenticación correctos usando Bearer token
- Parseo de respuestas JSON básico

## Configuración

### 1. API Key de Paddle
En el archivo `example/lib/main.dart`, reemplaza la línea:
```dart
final configuration = PurchasesConfiguration("pdl_your_paddle_api_key_here");
```

Con tu API key real de Paddle que obtienes del dashboard de RevenueCat (debe comenzar con "pdl_").

### 2. Compilación
```bash
cd example
flutter build windows
```

### 3. Ejecución
```bash
cd example
flutter run -d windows
```

## Archivos Principales

### C++ Implementation
- `windows/purchases_flutter_plugin.cpp` - Plugin principal
- `windows/http_client.cpp` - Cliente HTTP usando WinINet
- `windows/revenuecat_api.cpp` - Wrapper para la API de RevenueCat
- `windows/CMakeLists.txt` - Configuración de build

### Headers
- `windows/include/purchases_flutter/purchases_flutter_plugin.h`
- `windows/http_client.h`
- `windows/revenuecat_api.h`

## Cómo Funciona

### 1. Configuración (`configure`)
```cpp
// Almacena la API key de Paddle
api_key_ = apiKey;
// Inicializa el cliente de RevenueCat API
revenuecat_api_ = std::make_unique<RevenueCatApi>(apiKey);
```

### 2. Login (`logIn`)
```cpp
// 1. Intenta obtener usuario existente con GET
auto getResponse = http_client_.Get(getUrl, CreateAuthHeaders());

// 2. Si no existe, crea usuario con POST
if (!userExists) {
    auto postResponse = http_client_.Post(postUrl, postBody, CreateAuthHeaders());
}
```

### 3. Cliente HTTP
```cpp
// Usa WinINet para peticiones HTTPS
HINTERNET hInternet = InternetOpenA("RevenueCat-Windows-SDK/1.0", ...);
HINTERNET hConnect = InternetConnectA(hInternet, "api.revenuecat.com", ...);
HINTERNET hRequest = HttpOpenRequestA(hConnect, method, path, ...);
```

## Logs de Debug

La implementación incluye logs detallados que puedes ver en la consola:

```
RevenueCat configured with API key: pdl_xxxx...
App User ID: test_user_123
Making GET request to: https://api.revenuecat.com/v1/subscribers/test_user_123
Connecting to host: api.revenuecat.com
Path: /v1/subscribers/test_user_123
Adding headers: Authorization: Bearer pdl_xxxx...
Status code: 200
Response body: {"subscriber": {...}}
Login successful. Created: false
```

## Limitaciones Actuales

1. **Parsing JSON**: Implementación básica de parseo de respuestas
2. **Error Handling**: Manejo básico de errores HTTP
3. **Async Operations**: Peticiones HTTP son sincrónicas
4. **Certificados**: Usa certificados del sistema Windows

## Roadmap

- [ ] Implementar parser JSON completo
- [ ] Agregar manejo de errores robusto
- [ ] Implementar peticiones asíncronas
- [ ] Agregar soporte para más métodos (getCustomerInfo, etc.)
- [ ] Implementar caché local de CustomerInfo
- [ ] Agregar tests unitarios

## Diferencias con Android/iOS

A diferencia de Android/iOS que usan los SDKs nativos de RevenueCat, esta implementación:
- Hace peticiones HTTP directas a la API REST de RevenueCat
- Usa la public API key de Paddle en lugar de secret API keys
- Implementa un subset limitado de funcionalidades

## Verificación en Dashboard

Después de hacer `logIn`, deberías ver el usuario activo en el dashboard de RevenueCat:
1. Ve a https://app.revenuecat.com
2. Selecciona tu app
3. Ve a "Customers" 
4. Busca el `app_user_id` que usaste en el logIn

## Contribución

Este es un fork específico para agregar soporte real de RevenueCat en Windows. Para contribuir:

1. Fork el repositorio
2. Crea una rama feature
3. Implementa cambios
4. Agrega tests
5. Crea Pull Request

## Licencia

Mismo que el proyecto original de RevenueCat Flutter SDK.
