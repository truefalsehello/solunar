#include "stdafx.h"
#include "Common.h"
#include "WebKitBrowserWindow.h"
#include <wtf/win/SoftLinking.h>
#include "WebkitBrowser.h"

BrowserMiddleware::BrowserMiddleware(HWND parentHWND):m_parentHWND(parentHWND)
{
	::SetProcessDPIAware();

	auto factory = WebKitBrowserWindow::create;
	m_mainWindow = &MainWindow::create().leakRef();
	static_cast<MainWindow*>(m_mainWindow)->init(factory, m_parentHWND);
}

HWND WINAPI BrowserMiddleware::GetBrowserHWND()
{
	if (m_mainWindow) {
		return static_cast<MainWindow*>(m_mainWindow)->browserHwnd();
	}
	else {
		return nullptr;
	}
}

void WINAPI BrowserMiddleware::loadURL(std::wstring url)
{
	if(m_mainWindow){
		static_cast<MainWindow*>(m_mainWindow)->loadURL(url);
	}
}

void BrowserMiddleware::resetZoom()
{
	if (m_mainWindow) {
		static_cast<MainWindow*>(m_mainWindow)->resetZoom();
	}
}
