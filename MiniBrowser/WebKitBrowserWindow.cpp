#include "stdafx.h"
#include "WebKitBrowserWindow.h"

#include "Common.h"
#include "Common2.h"
#include <WebCore/GDIUtilities.h>
#include <WebKit/WKAuthenticationChallenge.h>
#include <WebKit/WKAuthenticationDecisionListener.h>
#include <WebKit/WKCertificateInfoCurl.h>
#include <WebKit/WKContextConfigurationRef.h>
#include <WebKit/WKCredential.h>
#include <WebKit/WKDownloadClient.h>
#include <WebKit/WKDownloadRef.h>
#include <WebKit/WKFrameInfoRef.h>
#include <WebKit/WKHTTPCookieStoreRef.h>
#include <WebKit/WKInspector.h>
#include <WebKit/WKNavigationResponseRef.h>
#include <WebKit/WKPreferencesRefPrivate.h>
#include <WebKit/WKProtectionSpace.h>
#include <WebKit/WKProtectionSpaceCurl.h>
#include <WebKit/WKSecurityOriginRef.h>
#include <WebKit/WKURLResponse.h>
#include <WebKit/WKWebsiteDataStoreConfigurationRef.h>
#include <WebKit/WKWebsiteDataStoreRef.h>
#include <WebKit/WKWebsiteDataStoreRefCurl.h>
#include <filesystem>
#include <sstream>
#include <vector>

std::wstring createString(WKURLRef wkURL)
{
    if (!wkURL)
        return { };
    WKRetainPtr<WKStringRef> url = adoptWK(WKURLCopyString(wkURL));
    return createString(url.get());
}

std::string createUTF8String(const wchar_t* src, size_t srcLength)
{
    int length = WideCharToMultiByte(CP_UTF8, 0, src, srcLength, 0, 0, nullptr, nullptr);
    std::vector<char> buffer(length);
    size_t actualLength = WideCharToMultiByte(CP_UTF8, 0, src, srcLength, buffer.data(), length, nullptr, nullptr);
    return { buffer.data(), actualLength };
}

WKRetainPtr<WKStringRef> createWKString(_bstr_t str)
{
    auto utf8 = createUTF8String(str, str.length());
    return adoptWK(WKStringCreateWithUTF8CString(utf8.data()));
}

WKRetainPtr<WKStringRef> createWKString(const std::wstring& str)
{
    auto utf8 = createUTF8String(str.c_str(), str.length());
    return adoptWK(WKStringCreateWithUTF8CString(utf8.data()));
}

WKRetainPtr<WKStringRef> createWKString(const wchar_t* str)
{
    return createWKString(std::wstring(str));
}

WKRetainPtr<WKURLRef> createWKURL(_bstr_t str)
{
    auto utf8 = createUTF8String(str, str.length());
    return adoptWK(WKURLCreateWithUTF8CString(utf8.data()));
}

WKRetainPtr<WKURLRef> createWKURL(const std::wstring& str)
{
    auto utf8 = createUTF8String(str.c_str(), str.length());
    return adoptWK(WKURLCreateWithUTF8CString(utf8.data()));
}

WKRetainPtr<WKStringRef> injectedBundlePath()
{
    auto module = GetModuleHandle(nullptr);
    std::wstring path;
    for (;;) {
        path.resize(path.size() + MAX_PATH);
        auto copied = GetModuleFileName(module, path.data(), path.size());
        if (copied < path.size()) {
            path.resize(copied);
            break;
        }
    }
    path = std::filesystem::path(path).replace_filename("MiniBrowserInjectedBundle.dll");
    return createWKString(path);
}

Ref<BrowserWindow> WebKitBrowserWindow::create(BrowserWindowClient& client, HWND mainWnd)
{
    auto websiteDataStoreConf = adoptWK(WKWebsiteDataStoreConfigurationCreate());

    auto websiteDataStore = adoptWK(WKWebsiteDataStoreCreateWithConfiguration(websiteDataStoreConf.get()));

    auto contextConf = adoptWK(WKContextConfigurationCreate());
    WKContextConfigurationSetInjectedBundlePath(contextConf.get(), injectedBundlePath().get());

    auto context = adoptWK(WKContextCreateWithConfiguration(contextConf.get()));

    auto preferences = adoptWK(WKPreferencesCreate());
    WKPreferencesSetMediaCapabilitiesEnabled(preferences.get(), false);
    WKPreferencesSetDeveloperExtrasEnabled(preferences.get(), true);
    WKPreferencesSetAcceleratedCompositingEnabled(preferences.get(), true);
    WKPreferencesSetAcceleratedDrawingEnabled(preferences.get(), true);
    WKPreferencesSetAccelerated2DCanvasEnabled(preferences.get(), true);

    auto pageConf = adoptWK(WKPageConfigurationCreate());
    WKPageConfigurationSetWebsiteDataStore(pageConf.get(), websiteDataStore.get());
    WKPageConfigurationSetContext(pageConf.get(), context.get());
    WKPageConfigurationSetPreferences(pageConf.get(), preferences.get());

    return adoptRef(*new WebKitBrowserWindow(client, pageConf.get(), mainWnd));
}

void WebKitBrowserWindow::resetZoom()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageSetPageZoomFactor(page, WebCore::deviceScaleFactorForWindow(hwnd()));
}

WebKitBrowserWindow::WebKitBrowserWindow(BrowserWindowClient& client, WKPageConfigurationRef pageConf, HWND mainWnd)
    : m_client(client)
    , m_hMainWnd(mainWnd)
{
    RECT rect = { };
    m_view = adoptWK(WKViewCreate(rect, pageConf, mainWnd));
    WKViewSetIsInWindow(m_view.get(), true);

    auto page = WKViewGetPage(m_view.get());

    WKPageUIClientV13 uiClient = { };
    uiClient.base.version = 13;
    uiClient.base.clientInfo = this;
    uiClient.createNewPage = nullptr;
    uiClient.close = close;
    uiClient.didNotHandleKeyEvent = didNotHandleKeyEvent;
    uiClient.getWindowFrame = getWindowFrame;
    uiClient.runJavaScriptAlert = runJavaScriptAlert;
    uiClient.runJavaScriptConfirm = runJavaScriptConfirm;
    uiClient.runJavaScriptPrompt = runJavaScriptPrompt;
    WKPageSetPageUIClient(page, &uiClient.base);

    WKPageStateClientV0 stateClient = { };
    stateClient.base.version = 0;
    stateClient.base.clientInfo = this;
    stateClient.didChangeTitle = didChangeTitle;
    stateClient.didChangeIsLoading = didChangeIsLoading;
    stateClient.didChangeEstimatedProgress = didChangeEstimatedProgress;
    stateClient.didChangeActiveURL = didChangeActiveURL;
    WKPageSetPageStateClient(page, &stateClient.base);

    resetZoom();
}

HWND WebKitBrowserWindow::hwnd()
{
    return WKViewGetWindow(m_view.get());
}

HRESULT WebKitBrowserWindow::loadURL(const BSTR& url)
{
    auto page = WKViewGetPage(m_view.get());
    WKPageLoadURL(page, createWKURL(_bstr_t(url)).get());
    return true;
}

void WebKitBrowserWindow::reload()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageReload(page);
}

void WebKitBrowserWindow::setUserAgent(_bstr_t& customUAString)
{
    auto page = WKViewGetPage(m_view.get());
    auto ua = createWKString(customUAString);
    WKPageSetCustomUserAgent(page, ua.get());
}

static WebKitBrowserWindow& toWebKitBrowserWindow(const void* clientInfo)
{
    return *const_cast<WebKitBrowserWindow*>(static_cast<const WebKitBrowserWindow*>(clientInfo));
}

void WebKitBrowserWindow::didChangeTitle(const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    auto page = WKViewGetPage(thisWindow.m_view.get());
    WKRetainPtr<WKStringRef> title = adoptWK(WKPageCopyTitle(page));
    std::wstring titleString = createString(title.get()) + L" [WebKit]";
    SetWindowText(thisWindow.m_hMainWnd, titleString.c_str());
}

void WebKitBrowserWindow::didChangeIsLoading(const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    thisWindow.m_client.progressFinished();
}

void WebKitBrowserWindow::didChangeEstimatedProgress(const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    auto page = WKViewGetPage(thisWindow.m_view.get());
    thisWindow.m_client.progressChanged(WKPageGetEstimatedProgress(page));
}

void WebKitBrowserWindow::didChangeActiveURL(const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    auto page = WKViewGetPage(thisWindow.m_view.get());
    WKRetainPtr<WKURLRef> url = adoptWK(WKPageCopyActiveURL(page));
    thisWindow.m_client.activeURLChanged(createString(url.get()));
}

void WebKitBrowserWindow::close(WKPageRef, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    PostMessage(thisWindow.hwnd(), WM_CLOSE, 0, 0);
}


void WebKitBrowserWindow::didNotHandleKeyEvent(WKPageRef, WKNativeEventPtr event, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    PostMessage(thisWindow.m_hMainWnd, event->message, event->wParam, event->lParam);
}

WKRect WebKitBrowserWindow::getWindowFrame(WKPageRef, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    RECT rect;
    GetWindowRect(thisWindow.hwnd(), &rect);
    return WKRectMake(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

void WebKitBrowserWindow::runJavaScriptAlert(WKPageRef, WKStringRef alertText, WKFrameRef, WKSecurityOriginRef securityOrigin, WKPageRunJavaScriptAlertResultListenerRef listener, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    std::wstring title = L"Alert: ";
    title += createString(adoptWK(WKSecurityOriginCopyToString(securityOrigin)).get());
    auto text = createString(alertText);
    MessageBox(thisWindow.m_hMainWnd, text.c_str(), title.c_str(), MB_OK);
    WKPageRunJavaScriptAlertResultListenerCall(listener);
}

void WebKitBrowserWindow::runJavaScriptConfirm(WKPageRef, WKStringRef message, WKFrameRef, WKSecurityOriginRef securityOrigin, WKPageRunJavaScriptConfirmResultListenerRef listener, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    std::wstring title = L"Confirm: ";
    title += createString(adoptWK(WKSecurityOriginCopyToString(securityOrigin)).get());
    auto text = createString(message);
    bool result = MessageBox(thisWindow.m_hMainWnd, text.c_str(), title.c_str(), MB_OKCANCEL) == IDOK;
    WKPageRunJavaScriptConfirmResultListenerCall(listener, result);
}

void WebKitBrowserWindow::runJavaScriptPrompt(WKPageRef, WKStringRef message, WKStringRef defaultValue, WKFrameRef, WKSecurityOriginRef securityOrigin, WKPageRunJavaScriptPromptResultListenerRef listener, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    std::wstring title = L"Prompt: ";
    title += createString(adoptWK(WKSecurityOriginCopyToString(securityOrigin)).get());
    auto text = createString(message);
    text += L"\nDefault Value: " + createString(defaultValue);
    MessageBox(thisWindow.m_hMainWnd, text.c_str(), title.c_str(), MB_OK);
    WKPageRunJavaScriptPromptResultListenerCall(listener, defaultValue);
}