#pragma once

#define WM_PROGRESS_CHANGE (WM_USER + 1)

class BrowserMiddleware {
public:
	BrowserMiddleware(HWND parentHWND);
	HWND WINAPI GetParentHWND() {
		return m_parentHWND;
	};
	HWND WINAPI GetBrowserHWND();
	void WINAPI loadURL(std::wstring url);
	void resetZoom();
private:
	HWND m_parentHWND{ nullptr };
	void* m_mainWindow{ nullptr };
};