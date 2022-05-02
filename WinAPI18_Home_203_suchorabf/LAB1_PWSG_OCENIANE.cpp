// LAB1_PWSG_OCENIANE.cpp : Defines the entry point for the application.
//
#pragma comment(lib, "Msimg32.lib")
#include "framework.h"
#include "LAB1_PWSG_OCENIANE.h"
#include <string>
#include <fstream>
#include <iostream>
#include <dwmapi.h>
#define buff_size 1024
#define MAX_LOADSTRING 100
#define tile_0 RGB(204, 192, 174)
#define tile_2 RGB(238, 228, 198)
#define tile_4 RGB(239, 225, 218)
#define tile_8 RGB(243, 179, 124)
#define tile_16 RGB(246, 153, 100)
#define tile_32 RGB(246, 125, 98)
#define tile_64 RGB(247, 93, 60)
#define tile_128 RGB(237, 206, 116)
#define tile_256 RGB(239, 204, 98)
#define tile_512 RGB(243, 201, 85)
#define tile_1024 RGB(238, 200, 72)
#define tile_2048 RGB(239, 192, 47)

// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name
WCHAR childClass[] = L"child";
WCHAR pointsClass[] = L"points";
WCHAR cloneClass[] = L"clone";
WCHAR winClass[] = L"win";
int centerX;
int centerY;
HWND hWnd2;
HWND hWnd1;
HWND points_window;
HWND points2_window;
HWND win_window1;
HWND win_window2;
int pos1x = 0;
int pos1y = 0;
int pos2x = 0;
int pos2y = 0;
int goal = 2048;
int points = 0;
int board[4][4];
int animations[4][4];
HWND* tiles[4][4];
HWND* tiles2[4][4];
std::ofstream outfile;
bool won = false;
bool firstRestoration = false;
// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_points(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_win(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void getColumnRow(LONG l, int* column, int* row)
{
	*column = l % 10;
	*row = l / 10;
}

void saveGame()
{
	outfile.open("config.cfg", std::ofstream::out | std::ofstream::trunc);
	outfile << points << std::endl;
	outfile << goal << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			outfile << board[i][j] << " ";
		}
	}
	outfile << std::endl << won;
	outfile.close();
}

void addRandom2(int board[4][4], bool update)
{
	while (1)
	{
		int i = rand() % 4;
		int j = rand() % 4;
		if (board[i][j] == 0)
		{
			board[i][j] = 2;
			if (update)
			{
				animations[i][j] = -4;
				InvalidateRect(*tiles[i][j], NULL, TRUE);
				UpdateWindow(*tiles[i][j]);
			}
			break;
		}
	}
}

void Win()
{
	OutputDebugString(L"WON!!");
	ShowWindow(win_window1, SW_SHOW);
	ShowWindow(win_window2, SW_SHOW);
}

bool checkLose()
{
	bool lost = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (board[i][j] == 0 ||
				i - 1 >= 0 && board[i][j] == board[i - 1][j] ||
				i + 1 < 4 && board[i][j] == board[i + 1][j] ||
				j - 1 >= 0 && board[i][j] == board[i][j - 1] ||
				j + 1 < 4 && board[i][j] == board[i][j + 1])
			{
				lost = false;
				break;
			}
		}
		if (!lost) break;
	}
	return lost;
}

void processInput(char direction)
{
	if (won) return;
	int moved = -1;
	switch (direction)
	{
	case 'w':
		for (int i = 0; i < 4; i++)
		{
			for (int j = 1; j < 4; j++)
			{
				int changes[] = {0, 0, 0, 0};
				for (int k = j - 1; k >= 0; k--)
				{
					if (board[k][i] == 0 && board[k + 1][i] != 0)
					{
						board[k][i] = board[k + 1][i];
						board[k + 1][i] = 0;
						moved = 1;
					}
					else if (board[k][i] == board[k + 1][i] && board[k + 1][i] != 0 && changes[k] == 0 && changes[k + 1]
						== 0)
					{
						changes[k] = 1;
						board[k][i] *= 2;
						animations[k][i] = 4;
						board[k + 1][i] = 0;
						points += board[k][i];
						if (board[k][i] == goal) won = true;
						moved = 1;
					}
					else
					{
						break;
					}
				}
			}
		}
		break;
	case 'a':
		for (int i = 0; i < 4; i++)
		{
			int changes[] = {0, 0, 0, 0};
			for (int j = 1; j < 4; j++)
			{
				for (int k = j - 1; k >= 0; k--)
				{
					if (board[i][k] == 0 && board[i][k + 1] != 0)
					{
						board[i][k] = board[i][k + 1];
						board[i][k + 1] = 0;
						moved = 1;
					}
					else if (board[i][k] == board[i][k + 1] && board[i][k + 1] != 0 && changes[k] == 0 && changes[k + 1]
						== 0)
					{
						changes[k] = 1;
						board[i][k] *= 2;
						animations[i][k] = 4;
						board[i][k + 1] = 0;
						points += board[i][k];
						if (board[i][k] == goal) won = true;
						moved = 1;
					}
					else
					{
						break;
					}
				}
			}
		}
		break;
	case 's':
		for (int i = 0; i < 4; i++)
		{
			int changes[] = {0, 0, 0, 0};
			for (int j = 2; j >= 0; j--)
			{
				for (int k = j + 1; k < 4; k++)
				{
					if (board[k][i] == 0 && board[k - 1][i] != 0)
					{
						board[k][i] = board[k - 1][i];
						board[k - 1][i] = 0;
						moved = 1;
					}
					else if (board[k][i] == board[k - 1][i] && board[k - 1][i] != 0 && changes[k] == 0 && changes[k - 1]
						== 0)
					{
						board[k][i] *= 2;
						animations[k][i] = 4;
						board[k - 1][i] = 0;
						changes[k] = 1;
						points += board[k][i];
						if (board[k][i] == goal) won = true;
						moved = 1;
					}
					else
					{
						break;
					}
				}
			}
		}
		break;
	case 'd':
		for (int i = 0; i < 4; i++)
		{
			int changes[] = {0, 0, 0, 0};
			for (int j = 2; j >= 0; j--)
			{
				for (int k = j + 1; k < 4; k++)
				{
					if (board[i][k] == 0 && board[i][k - 1] != 0)
					{
						board[i][k] = board[i][k - 1];
						board[i][k - 1] = 0;
						moved = 1;
					}
					else if (board[i][k] == board[i][k - 1] && board[i][k - 1] != 0 && changes[k] == 0 && changes[k - 1]
						== 0)
					{
						changes[k] = 1;
						board[i][k] *= 2;
						animations[i][k] = 4;
						board[i][k - 1] = 0;
						points += board[i][k];
						if (board[i][k] == goal) won = true;
						moved = 1;
					}
					else
					{
						break;
					}
				}
			}
		}
		break;
	}
	if (moved > 0)
	{
		addRandom2(board, true);
		for (int i = 0; i < 5; i++)
		{
			InvalidateRect(hWnd1, NULL, FALSE);
			InvalidateRect(hWnd2, NULL, FALSE);
			UpdateWindow(hWnd1);
			UpdateWindow(hWnd2);
			if (i != 4)
				Sleep(1);
		}
	}

	bool lost = checkLose();
	if (won || lost)
	{
		Win();
	}
}

void newGame()
{
	won = false;
	ShowWindow(win_window1, SW_HIDE);
	ShowWindow(win_window2, SW_HIDE);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			board[i][j] = 0;
		}
	}
	points = 0;
	addRandom2(board, false);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	std::ifstream infile;
	infile.open("config.cfg");
	if (infile.is_open())
	{
		OutputDebugString(L"opened\n");

		infile >> points;
		infile >> goal;
		wchar_t buff[buff_size];
		_stprintf_s(buff, TEXT("%d"), points);
		OutputDebugString(buff);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				infile >> board[i][j];
			}
		}
		infile >> won;
		infile.close();
	}
	else
	{
		newGame();
		OutputDebugString(L"not opened\n");
	}

	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	centerX = (rc.left + rc.right + 1) / 2;
	centerY = (rc.top + rc.bottom + 1) / 2;
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1PWSGOCENIANE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			animations[i][j] = 0;
		}
	}
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1PWSGOCENIANE));

	MSG msg;

	bool lost = checkLose();
	if (won || lost)
	{
		Win();
	}
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(250, 247, 238));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1PWSGOCENIANE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	WNDCLASSEXW wcex2;

	wcex2.cbSize = sizeof(WNDCLASSEX);

	wcex2.style = CS_HREDRAW | CS_VREDRAW;
	wcex2.lpfnWndProc = WndProc2;
	wcex2.cbClsExtra = 0;
	wcex2.cbWndExtra = 0;
	wcex2.hInstance = hInstance;
	wcex2.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex2.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MYICON));
	wcex2.hbrBackground = CreateSolidBrush(tile_0);
	wcex2.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1PWSGOCENIANE);
	wcex2.lpszClassName = childClass;
	wcex2.hIconSm = LoadIcon(wcex2.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex2);
	WNDCLASSEXW wcex3;

	wcex3.cbSize = sizeof(WNDCLASSEX);

	wcex3.style = CS_HREDRAW | CS_VREDRAW;
	wcex3.lpfnWndProc = WndProc3;
	wcex3.cbClsExtra = 0;
	wcex3.cbWndExtra = 0;
	wcex3.hInstance = hInstance;
	wcex3.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex3.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex3.hbrBackground = CreateSolidBrush(RGB(250, 247, 238));
	wcex3.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1PWSGOCENIANE);
	wcex3.lpszClassName = cloneClass;
	wcex3.hIconSm = LoadIcon(wcex3.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex3);

	WNDCLASSEXW wcex4;
	wcex4.cbSize = sizeof(WNDCLASSEX);

	wcex4.style = CS_HREDRAW | CS_VREDRAW;
	wcex4.lpfnWndProc = WndProc_points;
	wcex4.cbClsExtra = 0;
	wcex4.cbWndExtra = 0;
	wcex4.hInstance = hInstance;
	wcex4.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex4.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex4.hbrBackground = CreateSolidBrush(tile_0);
	wcex4.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1PWSGOCENIANE);
	wcex4.lpszClassName = pointsClass;
	wcex4.hIconSm = LoadIcon(wcex4.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex4);

	WNDCLASSEXW wcex5;
	wcex5.cbSize = sizeof(WNDCLASSEX);

	wcex5.style = CS_HREDRAW | CS_VREDRAW;
	wcex5.lpfnWndProc = WndProc_win;
	wcex5.cbClsExtra = 0;
	wcex5.cbWndExtra = 0;
	wcex5.hInstance = hInstance;
	wcex5.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex5.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex5.hbrBackground = CreateSolidBrush(tile_0);
	wcex5.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1PWSGOCENIANE);
	wcex5.lpszClassName = winClass;
	wcex5.hIconSm = LoadIcon(wcex5.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wcex5);
	return RegisterClassExW(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	int area_width = 10 + (10 + 60) * 4;
	int area_height = 10 + (10 + 60) * 4 + 70;
	RECT rc = {0, 0, area_width, area_height};
	AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, TRUE, WS_EX_COMPOSITED);
	hWnd1 = CreateWindowEx(WS_EX_COMPOSITED, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	                       centerX - (rc.right - rc.left), centerY - (rc.bottom - rc.top), rc.right - rc.left,
	                       rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd1)
	{
		return FALSE;
	}
	SetWindowLong(hWnd1, GWL_STYLE, GetWindowLong(hWnd1, GWL_STYLE) & ~WS_SIZEBOX);
	hWnd2 = CreateWindowEx(WS_EX_COMPOSITED, cloneClass, szTitle, WS_OVERLAPPED,
	                       centerX, centerY, rc.right - rc.left, rc.bottom - rc.top, hWnd1, nullptr, hInstance,
	                       nullptr);
	SetWindowLong(hWnd2, GWL_STYLE,GetWindowLong(hWnd2, GWL_STYLE) & ~WS_SIZEBOX);

	points_window = CreateWindowEx(WS_EX_COMPOSITED,pointsClass, szTitle, WS_CHILD, 10, 10, (10 + 60)*4-10, 60,
	                             hWnd1, nullptr, hInstance, nullptr);
	points2_window = CreateWindowEx(WS_EX_COMPOSITED, pointsClass, szTitle, WS_CHILD, 10, 10, (10 + 60) * 4 - 10, 60,
	                              hWnd2, nullptr, hInstance, nullptr);
	win_window1 = CreateWindowEx(WS_EX_COMPOSITED, winClass, szTitle, WS_POPUP | WS_OVERLAPPED, 0, 0, area_width, area_height,
	                           hWnd1, nullptr, hInstance, nullptr);
	win_window2 = CreateWindowEx(WS_EX_COMPOSITED, winClass, szTitle, WS_POPUP | WS_OVERLAPPED, 0, 0, area_width, area_height,
	                           hWnd2, nullptr, hInstance, nullptr);
	SetWindowLong(win_window1, GWL_EXSTYLE,GetWindowLong(win_window1, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(win_window1, 0, (255 * 50) / 100, LWA_ALPHA);
	SetMenu(win_window1, NULL);
	SetWindowLong(win_window2, GWL_EXSTYLE,GetWindowLong(win_window1, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(win_window2, 0, (255 * 50) / 100, LWA_ALPHA);
	SetMenu(win_window2, NULL);

	RECT rcc;
	GetClientRect(hWnd1, &rcc);
	int width = rcc.right - rcc.left;
	int height = rcc.bottom - rcc.top;
	ClientToScreen(hWnd1, (LPPOINT)&rcc.left);
	ClientToScreen(hWnd1, (LPPOINT)&rcc.right);
	MoveWindow(win_window1, rcc.left, rcc.top, width, height, FALSE);
	GetClientRect(hWnd2, &rcc);
	ClientToScreen(hWnd2, (LPPOINT)&rcc.left);
	ClientToScreen(hWnd2, (LPPOINT)&rcc.right);
	MoveWindow(win_window2, rcc.left, rcc.top, width, height, FALSE);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			HWND child = CreateWindowEx(WS_EX_COMPOSITED, childClass, szTitle, WS_CHILD, 10*(i) + i*60, 70 + 10 * (j) + j * 60, 80,
			                          80,
			                          hWnd1, nullptr, hInstance, nullptr);
			SetMenu(child, NULL);
			ShowWindow(child, nCmdShow);
			SetWindowLong(child, GWLP_USERDATA, i + j * 10);
			tiles[j][i] = &child;
			child = CreateWindowEx(WS_EX_COMPOSITED, childClass, szTitle, WS_CHILD, 10 * (i) + i * 60, 70 + 10 * (j) + j * 60, 80,
			                     80,
			                     hWnd2, nullptr, hInstance, nullptr);
			SetWindowLong(child, GWLP_USERDATA, i + j * 10);
			tiles2[j][i] = &child;
			SetMenu(child, NULL);
			ShowWindow(child, nCmdShow);
		}
	}
	SetWindowPos(win_window2, GetNextWindow(hWnd2, GW_HWNDPREV), 600, 600, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow(hWnd2, nCmdShow);
	UpdateWindow(hWnd2);
	ShowWindow(points_window, nCmdShow);
	UpdateWindow(points_window);
	ShowWindow(points2_window, nCmdShow);
	UpdateWindow(points2_window);
	ShowWindow(hWnd1, nCmdShow);
	UpdateWindow(hWnd1);
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SYSCOMMAND:
		if((wParam & 0xFFF0) == SC_MAXIMIZE)
		{
			return 0;
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			ShowWindow(win_window1, SW_HIDE);
			ShowWindow(win_window2, SW_HIDE);
			// ShowWindow(hWnd1, SW_HIDE);
			ShowWindow(hWnd2, SW_HIDE);

		}
		else if (wParam == SIZE_RESTORED)
		{
			if(!firstRestoration)
			{
				firstRestoration = true;
				break;
			}
			OutputDebugString(L"RESTORED");
			ShowWindow(win_window1, SW_SHOW);
			ShowWindow(win_window2, SW_SHOW);
			ShowWindow(hWnd1, SW_SHOW);
			ShowWindow(hWnd2, SW_SHOW);
		}
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case ID_8:
				goal = 8;
				break;
			case ID_16:
				goal = 16;
				break;
			case ID_64:
				goal = 64;
				break;
			case ID_2048:
				goal = 2048;
				break;
			case IDM_NEW_GAME:
				newGame();
				InvalidateRect(hWnd1, NULL, TRUE);
				UpdateWindow(hWnd1);
				InvalidateRect(hWnd2, NULL, TRUE);
				UpdateWindow(hWnd2);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
		{
			RECT* rc = (RECT*)lParam;
			int cx = rc->right - (rc->right - rc->left) / 2;
			int cy = rc->bottom - (rc->bottom - rc->top) / 2;
			int c2x = 2 * centerX - cx - (rc->right - rc->left) / 2;
			int c2y = 2 * centerY - cy - (rc->bottom - rc->top) / 2;
			pos1x = cx;
			pos1y = cy;
			pos2x = 2 * centerX - cx;
			pos2y = 2 * centerY - cy;
			SetWindowPos(hWnd2, 0, c2x, c2y, 0, 0, SWP_NOSIZE);

			if (abs(pos1x - pos2x) < (rc->right - rc->left) - 15 && abs(pos1y - pos2y) < (rc->bottom - rc->top) - 8)
			{
				SetWindowLong(hWnd2, GWL_EXSTYLE,
				              GetWindowLong(hWnd2, GWL_EXSTYLE) | WS_EX_LAYERED);
				// Make this window 50% alpha
				SetLayeredWindowAttributes(hWnd2, 0, (255 * 50) / 100, LWA_ALPHA);
				// Show this window
				UpdateWindow(hWnd2);
			}
			else
			{
				SetWindowLong(hWnd2, GWL_EXSTYLE,
				              GetWindowLong(hWnd2, GWL_EXSTYLE) | WS_EX_LAYERED);
				// Make this window 50% alpha
				SetLayeredWindowAttributes(hWnd2, 0, (255 * 100) / 100, LWA_ALPHA);
				// Show this window
				UpdateWindow(hWnd2);
			}
			RECT rcc;
			GetClientRect(hWnd1, &rcc);
			int width = rcc.right - rcc.left;
			int height = rcc.bottom - rcc.top;
			ClientToScreen(hWnd1, (LPPOINT)&rcc.left);
			ClientToScreen(hWnd1, (LPPOINT)&rcc.right);
			MoveWindow(win_window1, rcc.left, rcc.top, width, height, FALSE);
			GetClientRect(hWnd2, &rcc);
			ClientToScreen(hWnd2, (LPPOINT)&rcc.left);
			ClientToScreen(hWnd2, (LPPOINT)&rcc.right);
			MoveWindow(win_window2, rcc.left, rcc.top, width, height, FALSE);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* info = (MINMAXINFO*)lParam;

			info->ptMaxSize = POINT{310, 350};
		}
		break;
	case WM_CHAR:
		processInput((char)wParam);
		break;
	case WM_CLOSE:
		saveGame();
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			int column, row;
			LONG l = GetWindowLong(hWnd, GWLP_USERDATA);
			getColumnRow(l, &column, &row);
			HBRUSH brush;
			HPEN pen;
			switch (board[row][column])
			{
			case 0:
				brush = CreateSolidBrush(tile_0);
				pen = CreatePen(PS_SOLID, 0, tile_0);
				break;
			case 2:
				brush = CreateSolidBrush(tile_2);
				pen = CreatePen(PS_SOLID, 0, tile_2);
				break;
			case 4:
				brush = CreateSolidBrush(tile_4);
				pen = CreatePen(PS_SOLID, 0, tile_4);
				break;
			case 8:
				brush = CreateSolidBrush(tile_8);
				pen = CreatePen(PS_SOLID, 0, tile_8);
				break;
			case 16:
				brush = CreateSolidBrush(tile_16);
				pen = CreatePen(PS_SOLID, 0, tile_16);
				break;
			case 32:
				brush = CreateSolidBrush(tile_32);
				pen = CreatePen(PS_SOLID, 0, tile_32);
				break;
			case 64:
				brush = CreateSolidBrush(tile_64);
				pen = CreatePen(PS_SOLID, 0, tile_64);
				break;
			case 128:
				brush = CreateSolidBrush(tile_128);
				pen = CreatePen(PS_SOLID, 0, tile_128);
				break;
			case 256:
				brush = CreateSolidBrush(tile_256);
				pen = CreatePen(PS_SOLID, 0, tile_256);
				break;
			case 512:
				brush = CreateSolidBrush(tile_512);
				pen = CreatePen(PS_SOLID, 0, tile_512);
				break;
			case 1024:
				brush = CreateSolidBrush(tile_1024);
				pen = CreatePen(PS_SOLID, 0, tile_1024);
				break;
			case 2048:
				brush = CreateSolidBrush(tile_2048);
				pen = CreatePen(PS_SOLID, 0, tile_2048);
				break;
			default:
				brush = CreateSolidBrush(tile_0);
				pen = CreatePen(PS_SOLID, 0, tile_0);
			}

			RECT rc;
			GetClientRect(hWnd, &rc);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(250, 247, 238)));
			HPEN oldPen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 0, RGB(250, 247, 238)));
			RoundRect(hdc, 0, 0, rc.right, rc.bottom, 18, 18);
			DeleteObject(SelectObject(hdc, brush));
			DeleteObject(SelectObject(hdc, pen));
			TCHAR s[300]; // where you put result
			_stprintf_s(s, TEXT("%d"), board[row][column]);
			int fontsize = 90;
			if (animations[row][column] < 0)
			{
				switch (animations[row][column]++)
				{
				case -1:
					RoundRect(hdc, 15, 15, rc.right - 15, rc.bottom - 15, 14, 14);
					fontsize = 80;
					break;
				case -2:
					RoundRect(hdc, 20, 20, rc.right - 20, rc.bottom - 20, 9, 9);
					fontsize = 90;
					break;
				case -3:
					RoundRect(hdc, 25, 25, rc.right - 25, rc.bottom - 25, 6, 6);
					fontsize = 110;
					break;
				case -4:
					RoundRect(hdc, 30, 30, rc.right - 30, rc.bottom - 30, 4, 4);
					fontsize = 130;
					break;
				}
			}
			else if (animations[row][column] > 0)
			{
				switch (animations[row][column]--)
				{
				case 1:
					RoundRect(hdc, 6, 6, rc.right - 6, rc.bottom - 6, 21, 21);
					break;
				case 2:
					RoundRect(hdc, 3, 3, rc.right - 3, rc.bottom - 3, 25, 25);
					break;
				case 3:
					RoundRect(hdc, 6, 6, rc.right - 6, rc.bottom - 6, 21, 21);
					break;
				case 4:
					RoundRect(hdc, 10, 10, rc.right - 10, rc.bottom - 10, 18, 18);
					break;
				}
			}
			else
			{
				RoundRect(hdc, 10, 10, rc.right - 10, rc.bottom - 10, 18, 18);
			}

			HFONT font = CreateFont(
				-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), fontsize), // Height
				0, // Width
				0, // Escapement
				0, // Orientation
				FW_BOLD, // Weight
				false, // Italic
				FALSE, // Underline

				0, // StrikeOut
				EASTEUROPE_CHARSET, // CharSet
				OUT_DEFAULT_PRECIS, // OutPrecision
				CLIP_DEFAULT_PRECIS, // ClipPrecision
				DEFAULT_QUALITY, // Quality
				DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
				_T(" Verdana ")); // Facename
			HFONT oldFont = (HFONT)SelectObject(hdc, font);
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
			if (board[row][column] != 0)
			{
				DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			SelectObject(hdc, oldBrush);
			SelectObject(hdc, oldPen);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(font);
			ReleaseDC(hWnd, hdc);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ERASEBKGND:
		return 1;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_points(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...

			HBRUSH brush = CreateSolidBrush(tile_0);
			HPEN pen = CreatePen(PS_SOLID, 0, tile_0);
			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
			HPEN oldPen = (HPEN)SelectObject(hdc, pen);
			RECT rc;
			GetClientRect(hWnd, &rc);
			RoundRect(hdc, 0, 0, rc.right, rc.bottom, 18, 18);
			TCHAR s[300]; // where you put result
			_stprintf_s(s, TEXT("%d"), points);

			HFONT font = CreateFont(
				-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 90), // Height
				0, // Width
				0, // Escapement
				0, // Orientation
				FW_BOLD, // Weight
				false, // Italic
				FALSE, // Underline

				0, // StrikeOut
				EASTEUROPE_CHARSET, // CharSet
				OUT_DEFAULT_PRECIS, // OutPrecision
				CLIP_DEFAULT_PRECIS, // ClipPrecision
				DEFAULT_QUALITY, // Quality
				DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
				_T(" Verdana ")); // Facename
			HFONT oldFont = (HFONT)SelectObject(hdc, font);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
			DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			SelectObject(hdc, oldBrush);
			SelectObject(hdc, oldPen);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(font);
			ReleaseDC(hWnd, hdc);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ERASEBKGND:
		return 1;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc_win(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			HBRUSH brush;
			HPEN pen;
			TCHAR s[300];
			if (won)
			{
				brush = CreateSolidBrush(RGB(0, 255, 0));
				pen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
				_stprintf_s(s, TEXT("WIN!"));
			}
			else
			{
				brush = CreateSolidBrush(RGB(255, 0, 0));
				pen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
				_stprintf_s(s, TEXT("GAME OVER"));
			}

			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
			HPEN oldPen = (HPEN)SelectObject(hdc, pen);
			RECT rc;
			GetClientRect(hWnd, &rc);
			Rectangle(hdc, 0, 0, rc.right, rc.bottom);

			HFONT font = CreateFont(
				-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 50), // Height
				0, // Width
				0, // Escapement
				0, // Orientation
				FW_BOLD, // Weight
				false, // Italic
				FALSE, // Underline

				0, // StrikeOut
				EASTEUROPE_CHARSET, // CharSet
				OUT_DEFAULT_PRECIS, // OutPrecision
				CLIP_DEFAULT_PRECIS, // ClipPrecision
				DEFAULT_QUALITY, // Quality
				DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
				_T(" Verdana ")); // Facename
			HFONT oldFont = (HFONT)SelectObject(hdc, font);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
			DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			SelectObject(hdc, oldBrush);
			SelectObject(hdc, oldPen);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(font);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case ID_8:
				goal = 8;
				break;
			case ID_16:
				goal = 16;
				break;
			case ID_64:
				goal = 64;
				break;
			case ID_2048:
				goal = 2048;
				break;
			case IDM_NEW_GAME:
				newGame();
				InvalidateRect(hWnd1, NULL, TRUE);
				UpdateWindow(hWnd1);
				InvalidateRect(hWnd2, NULL, TRUE);
				UpdateWindow(hWnd2);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_CHAR:
		processInput((char)wParam);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOVING:
		{
			RECT* rc = (RECT*)lParam;
			int cx = rc->right - (rc->right - rc->left) / 2;
			int cy = rc->bottom - (rc->bottom - rc->top) / 2;
			int c2x = 2 * centerX - cx - (rc->right - rc->left) / 2;
			int c2y = 2 * centerY - cy - (rc->bottom - rc->top) / 2;
			pos2x = cx;
			pos2y = cy;
			pos1x = 2 * centerX - cx;
			pos1y = 2 * centerY - cy;
			SetWindowPos(hWnd1, 0, c2x, c2y, 0, 0, SWP_NOSIZE);

			if (abs(pos1x - pos2x) < (rc->right - rc->left) - 15 && abs(pos1y - pos2y) < (rc->bottom - rc->top) - 8)
			{
				SetWindowLong(hWnd, GWL_EXSTYLE,
				              GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
				// Make this window 50% alpha
				SetLayeredWindowAttributes(hWnd, 0, (255 * 50) / 100, LWA_ALPHA);
				// Show this window
				UpdateWindow(hWnd);
			}
			else
			{
				SetWindowLong(hWnd, GWL_EXSTYLE,
				              GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
				// Make this window 50% alpha
				SetLayeredWindowAttributes(hWnd, 0, (255 * 100) / 100, LWA_ALPHA);
				// Show this window
				UpdateWindow(hWnd);
			}
			RECT rcc;
			GetClientRect(hWnd1, &rcc);
			int width = rcc.right - rcc.left;
			int height = rcc.bottom - rcc.top;
			ClientToScreen(hWnd1, (LPPOINT)&rcc.left);
			ClientToScreen(hWnd1, (LPPOINT)&rcc.right);
			MoveWindow(win_window1, rcc.left, rcc.top, width, height, FALSE);
			GetClientRect(hWnd2, &rcc);
			ClientToScreen(hWnd2, (LPPOINT)&rcc.left);
			ClientToScreen(hWnd2, (LPPOINT)&rcc.right);
			MoveWindow(win_window2, rcc.left, rcc.top, width, height, FALSE);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}