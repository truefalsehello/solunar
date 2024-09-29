#include "duilib/UIlib.h"
#include "solunar_form.h"
#include "webkit_browser.h"

const std::wstring SolunarForm::kClassName = L"Solunar";

SolunarForm::SolunarForm()
{
}


SolunarForm::~SolunarForm()
{
}

std::wstring SolunarForm::GetSkinFolder()
{
	return L"solunar";
}

std::wstring SolunarForm::GetSkinFile()
{
	return L"solunar.xml";
}

std::wstring SolunarForm::GetWindowClassName() const
{
	return kClassName;
}

LRESULT SolunarForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

ui::Control* SolunarForm::CreateControl(const std::wstring& pstrClass) {
	if (pstrClass == L"WebkitBrowser")
	{
		return new WebkitBrowser(GetHWND());
	}
	return NULL;
}

void SolunarForm::InitWindow()
{
	auto wb_baidu = static_cast<WebkitBrowser*>(FindControl(L"wb_baidu"));
	auto wb_douyin = static_cast<WebkitBrowser*>(FindControl(L"wb_douyin"));

	auto progress_baidu = static_cast<ui::Progress*>(FindControl(L"progress_baidu"));
	auto progress_douyin = static_cast<ui::Progress*>(FindControl(L"progress_douyin"));

	wb_baidu->SetUrl(L"http://baidu.com/", progress_baidu);
	wb_douyin->SetUrl(L"http://douyin.com/", progress_douyin);

	auto option_baidu = static_cast<ui::Option*>(FindControl(L"option_baidu"));
	auto option_douyin = static_cast<ui::Option*>(FindControl(L"option_douyin"));

	option_baidu->AttachSelect(nbase::Bind(&SolunarForm::OnSelected, this, std::placeholders::_1));
	option_douyin->AttachSelect(nbase::Bind(&SolunarForm::OnSelected, this, std::placeholders::_1));

	AddMessageFilter(this);
}

LRESULT SolunarForm::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_PROGRESS_CHANGE: {
		auto wb_baidu = static_cast<WebkitBrowser*>(FindControl(L"wb_baidu"));
		auto wb_douyin = static_cast<WebkitBrowser*>(FindControl(L"wb_douyin"));

		if (wb_baidu->isPage((HWND)lParam, wParam));
		else if (wb_douyin->isPage((HWND)lParam, wParam));
	}
		break;
	default:
		break;
	}
	return true;
}

bool SolunarForm::OnSelected(ui::EventArgs* arg)
{
	auto option = static_cast<ui::Option*>(arg->pSender);
	std::wstring text = option->GetText();
	auto tab_browser = static_cast<ui::TabBox*>(FindControl(L"tab_browser"));

	if (text == L"°Ù¶È") {
		tab_browser->SelectItem(0);
	}
	else if (text == L"¶¶Òô") {
		tab_browser->SelectItem(1);
	}

	return true;
}
