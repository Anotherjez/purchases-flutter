// Simple HTTP client for RevenueCat API calls
#include <windows.h>
#include <wininet.h>
#include <string>
#include <iostream>

#pragma comment(lib, "wininet.lib")

class SimpleHttpClient
{
public:
    static std::string MakeGetRequest(const std::string &apiKey, const std::string &endpoint)
    {
        std::string debugLog = "HTTP Request: " + endpoint + " with API key: " + apiKey.substr(0, 10) + "...\n";
        OutputDebugStringA(debugLog.c_str());

        HINTERNET hInternet = InternetOpenA("RevenueCat-Windows-SDK/1.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!hInternet)
        {
            OutputDebugStringA("Failed to initialize Internet connection\n");
            return "";
        }

        std::string fullUrl = "https://api.revenuecat.com/v1" + endpoint;
        OutputDebugStringA(("Full URL: " + fullUrl + "\n").c_str());

        HINTERNET hConnect = InternetOpenUrlA(hInternet, fullUrl.c_str(), nullptr, 0, INTERNET_FLAG_SECURE, 0);
        if (!hConnect)
        {
            DWORD error = GetLastError();
            OutputDebugStringA(("Failed to open URL, error: " + std::to_string(error) + "\n").c_str());
            InternetCloseHandle(hInternet);
            return "";
        }

        // Add authorization header
        std::string authHeader = "Authorization: Bearer " + apiKey;
        OutputDebugStringA(("Adding header: " + authHeader + "\n").c_str());

        BOOL headerResult = HttpAddRequestHeadersA(hConnect, authHeader.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);
        if (!headerResult)
        {
            DWORD error = GetLastError();
            OutputDebugStringA(("Failed to add headers, error: " + std::to_string(error) + "\n").c_str());
        }

        // Read response
        std::string response;
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            response.append(buffer, bytesRead);
        }

        OutputDebugStringA(("Response received: " + response + "\n").c_str());

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return response;
    }

    static std::string MakePostRequest(const std::string &apiKey, const std::string &endpoint, const std::string &postData)
    {
        std::string debugLog = "HTTP POST Request: " + endpoint + " with data: " + postData + "\n";
        OutputDebugStringA(debugLog.c_str());

        HINTERNET hInternet = InternetOpenA("RevenueCat-Windows-SDK/1.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!hInternet)
        {
            OutputDebugStringA("Failed to initialize Internet connection\n");
            return "";
        }

        HINTERNET hConnect = InternetConnectA(hInternet, "api.revenuecat.com", INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect)
        {
            OutputDebugStringA("Failed to connect to server\n");
            InternetCloseHandle(hInternet);
            return "";
        }

        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", ("/v1" + endpoint).c_str(), nullptr, nullptr, nullptr, INTERNET_FLAG_SECURE, 0);
        if (!hRequest)
        {
            OutputDebugStringA("Failed to create request\n");
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        // Add headers
        std::string authHeader = "Authorization: Bearer " + apiKey;
        std::string contentTypeHeader = "Content-Type: application/json";

        HttpAddRequestHeadersA(hRequest, authHeader.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);
        HttpAddRequestHeadersA(hRequest, contentTypeHeader.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);

        // Send request
        BOOL result = HttpSendRequestA(hRequest, nullptr, 0, (LPVOID)postData.c_str(), static_cast<DWORD>(postData.length()));
        if (!result)
        {
            DWORD error = GetLastError();
            OutputDebugStringA(("Failed to send request, error: " + std::to_string(error) + "\n").c_str());
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        // Read response
        std::string response;
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            response.append(buffer, bytesRead);
        }

        OutputDebugStringA(("Response received: " + response + "\n").c_str());

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return response;
    }
};
