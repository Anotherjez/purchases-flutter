import 'package:flutter/material.dart';
import 'package:purchases_flutter/purchases_flutter.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'RevenueCat Windows Test',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(title: 'RevenueCat Windows Test'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  MyHomePageState createState() => MyHomePageState();
}

class MyHomePageState extends State<MyHomePage> {
  String _status = 'Ready';
  String _apiKey = 'YOUR_API_KEY_HERE'; // Reemplaza con tu API key real
  String _userId = 'test_user_${DateTime.now().millisecondsSinceEpoch}';
  String _customerInfo = '';

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            TextField(
              decoration: InputDecoration(
                labelText: 'API Key',
                border: OutlineInputBorder(),
              ),
              controller: TextEditingController(text: _apiKey),
              onChanged: (value) {
                setState(() {
                  _apiKey = value;
                });
              },
            ),
            SizedBox(height: 16),
            TextField(
              decoration: InputDecoration(
                labelText: 'User ID',
                border: OutlineInputBorder(),
              ),
              controller: TextEditingController(text: _userId),
              onChanged: (value) {
                setState(() {
                  _userId = value;
                });
              },
            ),
            SizedBox(height: 16),
            ElevatedButton(
              onPressed: _configure,
              child: Text('Configure RevenueCat'),
            ),
            SizedBox(height: 16),
            ElevatedButton(
              onPressed: _login,
              child: Text('Login User'),
            ),
            SizedBox(height: 16),
            Text(
              'Status: $_status',
              style: TextStyle(fontWeight: FontWeight.bold),
            ),
            SizedBox(height: 16),
            Expanded(
              child: SingleChildScrollView(
                child: Text(
                  'Customer Info:\\n$_customerInfo',
                  style: TextStyle(fontFamily: 'monospace'),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Future<void> _configure() async {
    try {
      setState(() {
        _status = 'Configuring...';
      });

      await Purchases.configure(
        PurchasesConfiguration(_apiKey),
      );

      setState(() {
        _status = 'Configuration successful!';
      });
    } catch (e) {
      setState(() {
        _status = 'Configuration failed: $e';
      });
    }
  }

  Future<void> _login() async {
    try {
      setState(() {
        _status = 'Logging in...';
      });

      final result = await Purchases.logIn(_userId);

      setState(() {
        _status = 'Login successful! Created: ${result.created}';
        _customerInfo = '''
Original App User ID: ${result.customerInfo.originalAppUserId}
First Seen: ${result.customerInfo.firstSeen}
Request Date: ${result.customerInfo.requestDate}
Active Subscriptions: ${result.customerInfo.activeSubscriptions}
''';
      });
    } catch (e) {
      setState(() {
        _status = 'Login failed: $e';
        _customerInfo = '';
      });
    }
  }
}
