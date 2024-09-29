#pragma once

#include <windows.h>
#include <wtf/RefCounted.h>

class BrowserWindowClient {
public:
    virtual void progressChanged(double) = 0;
    virtual void progressFinished() = 0;
    virtual void activeURLChanged(std::wstring) = 0;
};

class BrowserWindow : public RefCounted<BrowserWindow> {
public:
    enum class FeatureType { Experimental, InternalDebug };

    virtual ~BrowserWindow() { };

    virtual HWND hwnd() = 0;

    virtual HRESULT loadURL(const BSTR& passedURL) = 0;
    virtual void reload() = 0;
    virtual bool usesLayeredWebView() const { return false; }

    void setUserAgent();
    virtual void setUserAgent(_bstr_t& customUAString) = 0;

    virtual void resetZoom() = 0;
};