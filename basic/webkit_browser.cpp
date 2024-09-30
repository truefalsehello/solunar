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
		::MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_patch, true);
		m_patch = (++m_patch) % 2;
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
		HDC hDC = pRender->GetDC();
		HDC hdcSrc = ::GetDC(m_hWnd);
		::StretchBlt(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top,
			hdcSrc, 0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, SRCCOPY);
		::ReleaseDC(m_hWnd, hdcSrc);
	}
	return;
}
