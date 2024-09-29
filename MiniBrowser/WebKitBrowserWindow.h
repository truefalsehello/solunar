#pragma once

#include "BrowserWindow.h"
#include "Common.h"
#include <WebKit/WKRetainPtr.h>
#include <WebKit/WebKit2_C.h>
#include <unordered_map>
#include <wtf/Ref.h>

class WebKitBrowserWindow : public BrowserWindow {
public:
    static Ref<BrowserWindow> create(BrowserWindowClient&, HWND mainWnd);
    void resetZoom() override;

private:
    WebKitBrowserWindow(BrowserWindowClient&, WKPageConfigurationRef, HWND mainWnd);

    HWND hwnd() override;

    HRESULT loadURL(const BSTR& url) override;
    void reload() override;

    void setUserAgent(_bstr_t&) override;

    static void didChangeTitle(const void*);
    static void didChangeIsLoading(const void*);
    static void didChangeEstimatedProgress(const void*);
    static void didChangeActiveURL(const void*);
    static void close(WKPageRef, const void*);
    static void didNotHandleKeyEvent(WKPageRef, WKNativeEventPtr, const void*);
    static WKRect getWindowFrame(WKPageRef, const void*);
    static void runJavaScriptAlert(WKPageRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptAlertResultListenerRef, const void*);
    static void runJavaScriptConfirm(WKPageRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptConfirmResultListenerRef, const void*);
    static void runJavaScriptPrompt(WKPageRef, WKStringRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptPromptResultListenerRef, const void*);

    BrowserWindowClient& m_client;
    WKRetainPtr<WKViewRef> m_view;
    HWND m_hMainWnd{ nullptr };
};