#pragma once
#include "/VSProject/solunar/MiniBrowser/WebkitBrowser.h"

class WebkitBrowser : public ui::Control
{
public:
	WebkitBrowser(HWND hwnd);
	~WebkitBrowser();

	virtual void SetInternVisible(bool bVisible = true);
	virtual void SetPos(ui::UiRect rc) override;

	void SetUrl(std::wstring url, ui::Progress *progress);
	void resetZoom();

	bool isPage(HWND hwnd, int value);

protected:
	HWND m_hWnd{ nullptr };
private:
	int m_pacth{0};
	BrowserMiddleware* m_browsermiddleware{ nullptr };
	std::wstring m_url;
	ui::Progress* m_progress{nullptr};
};