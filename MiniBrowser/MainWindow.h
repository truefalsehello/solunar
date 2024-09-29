#pragma once

#include "BrowserWindow.h"
#include <functional>
#include <memory>
#include <string>
#include <wtf/RefPtr.h>

class MainWindow final : public RefCounted<MainWindow>, public BrowserWindowClient {
public:
    using BrowserWindowFactory = std::function<Ref<BrowserWindow>(BrowserWindowClient&, HWND mainWnd)>;

    static Ref<MainWindow> create();

    bool init(BrowserWindowFactory, HWND);

    HWND hwnd() const { return m_hMainWnd; }
    HWND browserHwnd() const { return m_browserWindow->hwnd(); }
    BrowserWindow* browserWindow() const { return m_browserWindow.get(); }

    void resetZoom();
    void loadURL(std::wstring);

    // BrowserWindowClient
    void progressChanged(double) final;
    void progressFinished() final;
    void activeURLChanged(std::wstring) final;

private:

    HWND m_hMainWnd{ nullptr };
    RefPtr<BrowserWindow> m_browserWindow;
};