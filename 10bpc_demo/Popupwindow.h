class PopupWindow
{
public:

	PopupWindow();
	~PopupWindow();


	HRESULT CreatePopupWindow(bool isPattern, LPWSTR mediaFileName);

private:

	HINSTANCE m_hInstance = nullptr;
	HWND      m_hWindow = nullptr;

	LPWSTR    m_FileName = nullptr;
	bool      m_isPattern=true;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};