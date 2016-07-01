#pragma once
#include "ChiliWin.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <string>

// for granting special access to hWnd only for Graphics constructor
class HWNDKey
{
	friend Graphics::Graphics( HWNDKey& );
protected:
	HWND hWnd = nullptr;
};

class MainWindow : public HWNDKey
{
public:
	MainWindow( HINSTANCE hInst,wchar_t* pArgs );
	~MainWindow();
	MainWindow( const MainWindow& ) = delete;
	MainWindow( MainWindow&& ) = delete;
	MainWindow& operator=( const MainWindow& ) = delete;
	bool IsActive() const;
	void ShowMessageBox( const std::wstring& title,const std::wstring& message ) const;
	void Kill()
	{
		PostQuitMessage( 0 );
	}
	// returns false if quitting
	bool ProcessMessage();
	const std::wstring& GetArgs() const
	{
		return args;
	}
private:
	static LRESULT WINAPI _HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam );
	static LRESULT WINAPI _HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam );
	LRESULT HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam );
public:
	Keyboard kbd;
	Mouse mouse;
private:
	static constexpr wchar_t* wndClassName = L"Chili DirectX Framework Window";
	HINSTANCE hInst = nullptr;
	std::wstring args;
};