#include "stdafx.h"
#include "MainWindow.h"
#include "WebkitBrowser.h"

#include "Common.h"
#include "WebKitBrowserWindow.h"
#include <sstream>

static const wchar_t* kMiniBrowserRegistryKey = L"Software\\WebKit\\MiniBrowser";

Ref<MainWindow> MainWindow::create()
{
    return adoptRef(*new MainWindow());
}

bool MainWindow::init(BrowserWindowFactory factory, HWND hwnd)
{
    m_hMainWnd = hwnd;

    if (!m_hMainWnd)
        return false;

    m_browserWindow = factory(*this, m_hMainWnd);
    if (!m_browserWindow)
        return false;

    m_browserWindow->setUserAgent();

    return true;
}

void MainWindow::resetZoom()
{
    m_browserWindow->resetZoom();
}

void MainWindow::loadURL(std::wstring url)
{
    if (::PathFileExists(url.c_str()) || ::PathIsUNC(url.c_str())) {
        wchar_t fileURL[INTERNET_MAX_URL_LENGTH];
        DWORD fileURLLength = _countof(fileURL);

        if (SUCCEEDED(::UrlCreateFromPath(url.c_str(), fileURL, &fileURLLength, 0)))
            url = fileURL;
    }
    if (url.find(L"://") == url.npos)
        url = L"http://" + url;

    if (FAILED(m_browserWindow->loadURL(_bstr_t(url.c_str()))))
        return;

    SetFocus(m_browserWindow->hwnd());
}

void MainWindow::progressChanged(double progress)
{
    ::SendMessage(m_hMainWnd, WM_PROGRESS_CHANGE, progress * 100, (LPARAM)browserHwnd());
}

void MainWindow::progressFinished()
{

}

void MainWindow::activeURLChanged(std::wstring url)
{
}