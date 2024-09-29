#include "stdafx.h"
#include "BrowserWindow.h"

void BrowserWindow::setUserAgent()
{
    _bstr_t customUserAgent;
    customUserAgent = L"Mozilla/5.0 (iPhone; CPU iPhone OS 14_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0 Mobile/15E148 Safari/604.1";

    setUserAgent(customUserAgent);
}
