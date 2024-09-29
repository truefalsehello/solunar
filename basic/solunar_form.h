#pragma once

class SolunarForm : public ui::WindowImplBase
{
public:
	SolunarForm();
	~SolunarForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual ui::Control* CreateControl(const std::wstring& pstrClass);

	virtual void InitWindow() override;

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	static const std::wstring kClassName;

private:
	bool OnSelected(ui::EventArgs* arg);
};

