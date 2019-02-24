/******************************************************************************

	Description:

	Contains the source code for the GesturesTest application for Windows
	Mobile 6.5 Professional Edition.  Submitted to Code Project on 10/28/2009.

	Author:

	David Cole (cor2879@gmail.com)

	Version:

	1.0.0.0

	Platform:

	Windows Mobile 6.5 Professional Edition
	Win32
	C++
******************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <gesture.h>
#include <aygshell.h>

#define LABEL_WIDTH		240
#define LABEL_HEIGHT	125

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI OnGesture(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI OnColorStatic(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hinst = NULL;
HWND g_hwndLabel = NULL;
HWND g_stateLabel = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	static TCHAR szAppName[] = TEXT("BoardWm");

	HWND hwnd;
	MSG msg;
	WNDCLASS wndClass;

	ZeroMemory(&wndClass, sizeof(WNDCLASS));

	g_hinst = hInstance;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("There was a problem registering the Windows Class"),
			szAppName, MB_ICONERROR);

		return 0;
	}


	hwnd = CreateWindow(szAppName,					// Window Class Name
						TEXT("Board Controller"),	// Window Caption
						WS_VISIBLE,					// Window Style
						0,							// Initial X position
						0,							// Initial Y position
						CW_USEDEFAULT,				// Initial Width
						CW_USEDEFAULT,				// Initial Height
						NULL,						// Parent Window Handle
						NULL,						// Menu Handle
						g_hinst,					// Program Instance Handle
						NULL);						// Creation Parameters

	if (hwnd)
	{
		ShowWindow(hwnd, nShowCmd);
		UpdateWindow(hwnd);

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (hwnd)
		{
			CloseHandle(hwnd);
		}

		UnregisterClass(szAppName, hInstance);

		return msg.wParam;
	}

	return GetLastError();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		return OnCreate(hwnd, wParam, lParam);
	case WM_GESTURE:
		return OnGesture(hwnd, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return OnColorStatic(hwnd, wParam, lParam);
	case WM_ACTIVATE:
		if (WA_INACTIVE == (DWORD)wParam)
		{
			return SendMessage(hwnd, WM_DESTROY, 0,0);
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	case WM_SIZE:
		return OnSize(hwnd, wParam, lParam);
	case WM_CLOSE:
		return SendMessage(hwnd, WM_DESTROY, 0, 0);
	case WM_DESTROY:
		if (g_hwndLabel)
		{
			CloseHandle(g_hwndLabel);
		}
		if (g_stateLabel)
		{
			CloseHandle(g_stateLabel);
		}
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

LRESULT WINAPI OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (!hwnd)
	{
		return ERROR_INVALID_PARAMETER;
	}

	RECT rect;
	GetClientRect(hwnd, &rect);

	int iLabelLeft = (rect.right >> 1) - (LABEL_WIDTH >> 1);
	int iLabelTop = (rect.bottom >> 1) - (LABEL_HEIGHT >> 1);
	
	// Create a Text Label and center it on screen
	g_hwndLabel = CreateWindowEx(0,
								 TEXT("STATIC"),
								 0,
								 WS_CHILD | WS_VISIBLE | SS_CENTER,
								 iLabelLeft,
								 iLabelTop,
								 LABEL_WIDTH,
								 LABEL_HEIGHT,
								 hwnd,
								 NULL,
								 g_hinst,
								 NULL);

		g_stateLabel = CreateWindowEx(0,
								 TEXT("STATIC"),
								 0,
								 WS_CHILD | WS_VISIBLE | SS_LEFT,
								 iLabelLeft,
								 iLabelTop,
								 LABEL_WIDTH,
								 LABEL_HEIGHT,
								 hwnd,
								 NULL,
								 g_hinst,
								 NULL);

	// Create an empty menu bar for our app
	SHMENUBARINFO mbi;
	ZeroMemory(&mbi, sizeof(SHMENUBARINFO));
	mbi.cbSize = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	SHCreateMenuBar(&mbi);
	DrawMenuBar(hwnd);

	// Set the font
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	StringCchCopy(lf.lfFaceName, 64, TEXT("Arial"));

	HFONT hFont = CreateFontIndirect(&lf);
	SendMessage(g_hwndLabel, WM_SETFONT, (WPARAM)hFont, 0);
	SendMessage(g_stateLabel, WM_SETFONT, (WPARAM)hFont, 0);
	CloseHandle(hFont);

	// TODO: try to connect to bt spp

	return ERROR_SUCCESS;
}

LRESULT WINAPI OnGesture(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (!hwnd || !wParam || !lParam)
	{
		return ERROR_INVALID_PARAMETER;
	}

	GESTUREINFO gi;
	ZeroMemory(&gi, sizeof(GESTUREINFO));

	gi.cbSize = sizeof(GESTUREINFO);

	if (TKGetGestureInfo((HGESTUREINFO)lParam, &gi))
	{
		int iBufferSize = 128;
		PTSTR szGestureText = new TCHAR[iBufferSize];
		PTSTR szStateText = new TCHAR[iBufferSize];
		ZeroMemory(szGestureText, sizeof(TCHAR) * iBufferSize);
		ZeroMemory(szStateText, sizeof(TCHAR) * iBufferSize);

		switch(wParam)
		{
		/* No action on GID_BEGIN or GID_END events.  Otherwise
		   all we would ever see on the screen is GID_END :-)
		*/
		case GID_BEGIN:
			printf("GID_BEGIN\n");
			StringCchPrintf(szStateText, iBufferSize, TEXT("GID_BEGIN\nX: %d, Y: %d"),
					gi.ptsLocation.x, gi.ptsLocation.y);
			SetWindowText(g_stateLabel, szStateText);
			delete[] szStateText;
			// save start pos
			break;
		case GID_END:
			// send 40
			StringCchPrintf(szStateText, iBufferSize, TEXT("GID_END\nX: %d, Y: %d"),
					gi.ptsLocation.x, gi.ptsLocation.y);
			SetWindowText(g_stateLabel, szStateText);
			delete[] szStateText;
			return DefWindowProc(hwnd, WM_GESTURE, wParam, lParam);
			break;
		/* A pan event occurs when a user drags their finger (or stylus) across
		   the screen.
		*/
		case GID_PAN:
		{
			/* According to MSDN, a GID_PAN event is supposed to produce direction,
			   angle, and velocity data when the GF_INERTIA flag is flipped on.  I
			   have yet to be able to reproduce this behavior.
			*/
				StringCchPrintf(szGestureText, iBufferSize, TEXT("GID_PAN\nX: %d, Y: %d"),
					gi.ptsLocation.x, gi.ptsLocation.y);
				SetWindowText(g_hwndLabel, szGestureText);
				delete[] szGestureText;
			break;
		}
		/* A Hold event occurs when the user touches the screen and does not move their
		   finger (or the stylus) and also does not release the screen.  It usually takes
		   about two seconds for a touch event to register as a GID_HOLD message on my
		   phone.  MSDN states that the amount of time required is defined arbitrarily
		   at some lower level, possibly by the OEM.  Therefore the amount of time it
		   takes for a GID_HOLD message to appear may vary by device.
		*/
		case GID_HOLD:
			StringCchPrintf(szGestureText, iBufferSize, TEXT("GID_HOLD\nX: %d, Y: %d"),
				gi.ptsLocation.x, gi.ptsLocation.y);
			SetWindowText(g_hwndLabel, szGestureText);
			delete[] szGestureText;
			break;

		/* A Select event occurs when the user touches the screen.  It is comparable to 
		   left clicking a mouse.
		*/
		case GID_SELECT:
			StringCchPrintf(szGestureText, iBufferSize, TEXT("GID_SELECT\nX: %d, Y: %d"),
				gi.ptsLocation.x, gi.ptsLocation.y);
			SetWindowText(g_hwndLabel, szGestureText);
			delete[] szGestureText;
			break;

		/* A Double Select event occurs when the user double taps the screen.  That is,
		   quickly taps the screen twice.  It is comparable to double clicking the left
		   button of a mouse.
		*/
		case GID_DOUBLESELECT:
			StringCchPrintf(szGestureText, iBufferSize, TEXT("GID_DOUBLESELECT\nX: %d, Y: %d"),
				gi.ptsLocation.x, gi.ptsLocation.y);
			SetWindowText(g_hwndLabel, szGestureText);
			delete[] szGestureText;
			break;
		default:
			StringCchPrintf(szGestureText, iBufferSize, TEXT("default"));
			SetWindowText(g_hwndLabel, szGestureText);
			delete[] szGestureText;
			break;
		}

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_INVALID_PARAMETER;
	}
}

LRESULT WINAPI OnColorStatic(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush = CreateSolidBrush(RGBA(0,0,0,0));

	HWND hwndStatic = (HWND)lParam;
	HDC dc = (HDC)wParam;

	SetTextColor(dc, RGB(255, 255, 255));
	SetBkColor(dc, RGBA(0, 0, 0, 255));
	SetBkMode(dc, TRANSPARENT);

	return (LRESULT)hBrush;
}

LRESULT WINAPI OnSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (g_hwndLabel)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);

		int iLabelLeft = (rect.right >> 1) - (LABEL_WIDTH >> 1);
		int iLabelTop = (rect.bottom >> 1) - (LABEL_HEIGHT >> 1);

		MoveWindow(g_hwndLabel,
				   iLabelLeft,
				   iLabelTop,
				   LABEL_WIDTH,
				   LABEL_HEIGHT,
				   TRUE);
	}

	if (g_stateLabel)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);

		MoveWindow(g_stateLabel,
				   10,
				   40,
				   LABEL_WIDTH,
				   LABEL_HEIGHT,
				   TRUE);
	}

	return DefWindowProc(hwnd, WM_SIZE, wParam, lParam);
}