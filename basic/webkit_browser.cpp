#include "duilib/UIlib.h"
#include "/VSProject/solunar/MiniBrowser/WebkitBrowser.h"
#include "webkit_browser.h"

#pragma comment(lib, "D:/VSProject/duilib-netease/libs/x64/base_d.lib")
#pragma comment(lib, "D:/VSProject/duilib-netease/libs/x64/duilib_d.lib")
#pragma comment(lib, "D:/VSProject/solunar/x64/Debug/webkitbrowser.lib")

WebkitBrowser::WebkitBrowser(HWND hwnd)
{
	m_browsermiddleware = new BrowserMiddleware(hwnd);
	if (m_browsermiddleware)
		m_hWnd = m_browsermiddleware->GetBrowserHWND();
}

WebkitBrowser::~WebkitBrowser()
{
	if (m_browsermiddleware)
		delete m_browsermiddleware;
}

void WebkitBrowser::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);
	if (m_hWnd) {
		::ShowWindow(m_hWnd, bVisible);
	}
}

void WebkitBrowser::SetPos(ui::UiRect rc)
{
	__super::SetPos(rc);
	if (m_hWnd) {
		::SetWindowPos(m_hWnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void WebkitBrowser::SetUrl(std::wstring url, ui::Progress *progress)
{
	m_url = url;
	m_progress = progress;
	if (m_browsermiddleware)
		m_browsermiddleware->loadURL(url);
}

void WebkitBrowser::resetZoom()
{
	if (m_browsermiddleware)
		m_browsermiddleware->resetZoom();
}

bool WebkitBrowser::isPage(HWND hwnd, int value)
{
	if (m_hWnd == hwnd) {
		if (m_progress) {
			wchar_t progress[200];
			swprintf_s(progress, L"%s %lld\n", m_url.c_str(), value);
			OutputDebugString(progress);
			m_progress->SetValue(value);
		}
		return true;
	}
	return false;
}

void WebkitBrowser::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	if (m_hWnd) {
		MoveWindow(m_hWnd, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left + 1, m_rcPaint.bottom - m_rcPaint.top + 1, true);
	}
	return;
}
