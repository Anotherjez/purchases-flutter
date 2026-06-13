// Test script para verificar la implementación de RevenueCat Windows
// Este archivo muestra cómo probar el soporte real de RevenueCat

import 'package:flutter/material.dart';
import 'package:purchases_flutter/purchases_flutter.dart';

void main() {
  runApp(RevenueCatTestApp());
}

class RevenueCatTestApp extends StatelessWidget {
  const RevenueCatTestApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'RevenueCat Windows Test',
      theme: ThemeData(primarySwatch: Colors.blue),
      home: TestHomePage(),
    );
  }
}

class TestHomePage extends StatefulWidget {
  const TestHomePage({super.key});

  @override
  TestHomePageState createState() => TestHomePageState();
}

class TestHomePageState extends State<TestHomePage> {
  String _status = 'Not configured';
  String _appUserID = '';
  bool _isConfigured = false;
  bool _isLoggedIn = false;

  @override
  void initState() {
    super.initState();
    _configureRevenueCat();
  }

  Future<void> _configureRevenueCat() async {
    try {
      // 🔥 IMPORTANTE: Reemplaza con tu API key real de Paddle
      // Debe comenzar con "pdl_" y obtenerla del dashboard de RevenueCat
      const String paddleApiKey = "pdl_your_paddle_api_key_here";

      if (paddleApiKey == "pdl_your_paddle_api_key_here") {
        setState(() {
          _status =
              "❌ ERROR: Debes reemplazar 'pdl_your_paddle_api_key_here' con tu API key real de Paddle";
        });
        return;
      }

      final configuration = PurchasesConfiguration(paddleApiKey);
      await Purchases.configure(configuration);

      final appUserID = await Purchases.appUserID;

      setState(() {
        _appUserID = appUserID;
        _status = '✅ RevenueCat configurado exitosamente';
        _isConfigured = true;
      });
    } catch (e) {
      setState(() {
        _status = '❌ Error configurando RevenueCat: $e';
      });
    }
  }

  Future<void> _testLogin() async {
    if (!_isConfigured) {
      _showMessage('❌ Debes configurar RevenueCat primero');
      return;
    }

    try {
      // Genera un ID único para el test
      final testUserId = 'test_user_${DateTime.now().millisecondsSinceEpoch}';

      setState(() {
        _status = '🔄 Haciendo login...';
      });

      // 🔥 AQUÍ SE HACE LA PETICIÓN REAL A LA API DE REVENUECAT
      final result = await Purchases.logIn(testUserId);

      setState(() {
        _appUserID = result.customerInfo.originalAppUserId;
        _status = result.created
            ? '✅ Usuario creado exitosamente en RevenueCat'
            : '✅ Usuario ya existía en RevenueCat';
        _isLoggedIn = true;
      });

      _showMessage(
          '✅ Login exitoso! ${result.created ? 'Usuario creado' : 'Usuario existente'}');
    } catch (e) {
      setState(() {
        _status = '❌ Error en login: $e';
      });
      _showMessage('❌ Error en login: $e');
    }
  }

  Future<void> _testGetCustomerInfo() async {
    if (!_isLoggedIn) {
      _showMessage('❌ Debes hacer login primero');
      return;
    }

    try {
      await Purchases.getCustomerInfo();

      _showMessage('📊 Customer Info obtenida exitosamente');
    } catch (e) {
      _showMessage('❌ Error obteniendo customer info: $e');
    }
  }

  void _showMessage(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message)),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('RevenueCat Windows Test'),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Card(
              child: Padding(
                padding: EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Estado:',
                      style:
                          TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text(_status),
                    SizedBox(height: 16),
                    Text(
                      'App User ID:',
                      style:
                          TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text(_appUserID.isEmpty ? 'No asignado' : _appUserID),
                  ],
                ),
              ),
            ),
            SizedBox(height: 20),
            Text(
              'Pruebas:',
              style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
            SizedBox(height: 16),
            ElevatedButton(
              onPressed: _testLogin,
              child: Text('🔑 Probar Login'),
            ),
            SizedBox(height: 8),
            ElevatedButton(
              onPressed: _testGetCustomerInfo,
              child: Text('📊 Obtener Customer Info'),
            ),
            SizedBox(height: 20),
            Card(
              color: Colors.blue[50],
              child: Padding(
                padding: EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      '📝 Instrucciones:',
                      style:
                          TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text(
                      '1. Reemplaza "pdl_your_paddle_api_key_here" con tu API key real de Paddle\n'
                      '2. Ejecuta "flutter run -d windows"\n'
                      '3. Haz clic en "Probar Login"\n'
                      '4. Verifica en el dashboard de RevenueCat que el usuario aparece\n'
                      '5. Revisa los logs en la consola para debug',
                      style: TextStyle(fontSize: 14),
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

/* 
PASOS PARA PROBAR:

1. Obtén tu API key de Paddle:
   - Ve a https://app.revenuecat.com
   - Selecciona tu app
   - Ve a "App Settings" > "API Keys"
   - Copia la "Public API Key" de Paddle (comienza con "pdl_")

2. Reemplaza la API key en este archivo:
   const String paddleApiKey = "pdl_tu_api_key_aqui";

3. Ejecuta:
   flutter run -d windows

4. Haz clic en "Probar Login"

5. Verifica en el dashboard:
   - Ve a "Customers"
   - Busca el usuario con el ID generado
   - Debe aparecer como activo

6. Revisa los logs en la consola para debug

LOGS ESPERADOS:
🔧 Configurando RevenueCat con API key: pdl_xxxx...
✅ RevenueCat configurado. App User ID: $RCAnonymousID:xxx
🔑 Intentando login con usuario: test_user_1641234567890
Making GET request to: https://api.revenuecat.com/v1/subscribers/test_user_1641234567890
Status code: 404
Creating user. Status: 201
✅ Login exitoso: Usuario creado: true
*/
