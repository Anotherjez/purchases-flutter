import 'package:flutter/material.dart';
import 'package:purchases_flutter/purchases_flutter.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Purchases Flutter Windows Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  String _appUserID = '';
  String _status = 'Not configured';
  bool _isConfigured = false;

  @override
  void initState() {
    super.initState();
    _initializePurchases();
  }

  Future<void> _initializePurchases() async {
    try {
      // Configure Purchases with your API key
      final configuration = PurchasesConfiguration("your_api_key_here");
      await Purchases.configure(configuration);

      // Get the current app user ID
      final appUserID = await Purchases.appUserID;

      setState(() {
        _appUserID = appUserID;
        _status = 'Configured successfully';
        _isConfigured = true;
      });
    } catch (e) {
      setState(() {
        _status = 'Configuration failed: $e';
      });
    }
  }

  Future<void> _logIn() async {
    if (!_isConfigured) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Please configure Purchases first')),
      );
      return;
    }

    try {
      final result = await Purchases.logIn(
          'test_user_' + DateTime.now().millisecondsSinceEpoch.toString());

      setState(() {
        _appUserID = result.customerInfo.originalAppUserId;
        _status =
            'Logged in successfully. User ${result.created ? 'created' : 'exists'}.';
      });

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content: Text(
                'Login successful! User ${result.created ? 'created' : 'exists'}')),
      );
    } catch (e) {
      setState(() {
        _status = 'Login failed: $e';
      });

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Login failed: $e')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Purchases Flutter Windows Demo'),
        backgroundColor: Colors.blue,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Windows Support Status',
                      style:
                          TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text(
                      'This is a basic Windows implementation for development purposes. '
                      'Full RevenueCat functionality is not available on Windows.',
                      style: TextStyle(color: Colors.orange[700]),
                    ),
                  ],
                ),
              ),
            ),
            SizedBox(height: 16),
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Current Status',
                      style:
                          TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text('Status: $_status'),
                    if (_appUserID.isNotEmpty) ...[
                      SizedBox(height: 8),
                      Text('App User ID: $_appUserID'),
                    ],
                  ],
                ),
              ),
            ),
            SizedBox(height: 16),
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Available Methods',
                      style:
                          TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    SizedBox(height: 8),
                    Text('✓ configure() - Basic configuration'),
                    Text('✓ logIn() - User login with simulated response'),
                    Text('✓ appUserID - Get current user ID'),
                    SizedBox(height: 8),
                    Text(
                      'Note: Other methods like purchases, offerings, etc. are not implemented for Windows.',
                      style: TextStyle(color: Colors.grey[600], fontSize: 12),
                    ),
                  ],
                ),
              ),
            ),
            SizedBox(height: 24),
            Center(
              child: ElevatedButton(
                onPressed: _isConfigured ? _logIn : null,
                child: Text('Test Login'),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.blue,
                  padding: EdgeInsets.symmetric(horizontal: 24, vertical: 12),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
