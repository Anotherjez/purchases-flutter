# Windows Support for Purchases Flutter

This fork includes basic Windows support for the `purchases_flutter` plugin. Please note that this implementation is for development purposes only and provides limited functionality compared to the full RevenueCat SDK available on iOS and Android.

## Supported Methods on Windows

### ✅ Implemented
- `configure()` - Basic plugin configuration
- `logIn()` - User login with simulated response
- `appUserID` - Get current user ID

### ❌ Not Implemented
- All purchase-related methods
- Offerings and products
- Customer info syncing with RevenueCat backend
- Subscription management
- Most other RevenueCat features

## Installation for Windows

1. Ensure you have Flutter Windows support enabled:
   ```bash
   flutter config --enable-windows-desktop
   ```

2. Add the plugin to your `pubspec.yaml`:
   ```yaml
   dependencies:
     purchases_flutter:
       path: ../path/to/your/fork
   ```

3. Run flutter create to generate Windows files if needed:
   ```bash
   flutter create --platforms=windows .
   ```

## Usage Example

```dart
import 'package:purchases_flutter/purchases_flutter.dart';

// Configure the plugin
final configuration = PurchasesConfiguration("your_api_key_here");
await Purchases.configure(configuration);

// Log in a user
final result = await Purchases.logIn('user_123');
print('User logged in: ${result.customerInfo.originalAppUserId}');
print('User created: ${result.created}');

// Get current user ID
final appUserID = await Purchases.appUserID;
print('Current user ID: $appUserID');
```

## Important Notes

1. **Development Only**: This Windows implementation is for development and testing purposes. It does not connect to the RevenueCat backend.

2. **Simulated Responses**: All responses are simulated and don't reflect actual purchase or subscription data.

3. **Limited Functionality**: Most RevenueCat features are not available on Windows. Only basic user management is implemented.

4. **No Real Purchases**: Purchase methods will throw `UnsupportedPlatformException` or return errors.

## Error Handling

The plugin will show debug messages when running on Windows to remind you of the limited functionality:

```
purchases_flutter: Windows support is limited. Using simulated configuration.
purchases_flutter: Windows support is limited. Using simulated response.
```

## Building for Windows

To build your app for Windows:

```bash
flutter build windows
```

## Contributing

If you want to extend Windows support, you can:

1. Add more method implementations in `windows/purchases_flutter_plugin.cpp`
2. Update the corresponding method handlers in the header file
3. Test with the provided example app

## Technical Implementation Details

The Windows implementation includes:

- **C++ Plugin**: Native Windows plugin implementation
- **Method Channel**: Communication between Dart and C++
- **Simulated Responses**: Mock CustomerInfo and LogInResult objects
- **Basic State Management**: Tracks configuration and user ID

### File Structure

```
windows/
├── purchases_flutter_plugin.cpp    # Main plugin implementation
├── purchases_flutter_plugin.h      # Header file with declarations
└── ...                            # Other Windows-specific files
```

## Testing

Run the example app to test the Windows implementation:

```bash
cd example
flutter run -d windows
```

## Limitations

- No actual RevenueCat backend integration
- No real purchase processing
- No subscription management
- No offering/product fetching
- No customer info syncing
- No entitlement verification

This implementation allows you to develop and test your app on Windows while using the full RevenueCat functionality on mobile platforms.
