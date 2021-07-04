// FXFormat.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "FXFormat.h"
#include <shellapi.h>
#include <string>
using namespace std;
bool isLocal = false;
wchar_t  limitPath[MAX_PATH]=L"limit.dat";

//显示消息
void show(HWND hWnd, string str) {
	MessageBoxA(hWnd, str.c_str(), "提示", MB_OK);
}

//判断是否为文件夹
bool isDir(string path) {
	struct _stat buf = { 0 };
	_stat(path.c_str(), &buf);
	return buf.st_mode & _S_IFDIR;
}

//判断是否为文件
bool isFile(string path) {
	struct _stat buf = { 0 };
	_stat(path.c_str(), &buf);
	return buf.st_mode & _S_IFREG;
}

//根据文件路径获取所在文件夹的路径
string getFileDir(string path) {
	int i = path.find_last_of('\\');
	std::string p2 = path.substr(0, i);
	MessageBoxA(NULL, (const char *)p2.c_str(), "", MB_OK);
	return p2.c_str();
}

//获取剪切板内容
string getClipboardStr() {
	OpenClipboard(NULL);
	HGLOBAL hGlobal = NULL;
	hGlobal = ::GetClipboardData(CF_UNICODETEXT);
	wchar_t *pGlobal = (wchar_t *)::GlobalLock(hGlobal);
	CloseClipboard();
	return wtoc(pGlobal);
}

void setClipboardStr(string data) {
	if (OpenClipboard(NULL))    //打开剪切板
	{
		EmptyClipboard(); //清空剪切板
		HANDLE hclip;      //GlobalAlloc分配内存的句柄
		char *buf;         //返回的内存地址
		int size = data.length() + 1;
		hclip = GlobalAlloc(GMEM_MOVEABLE, size);//在堆上分配制定数目的字节
		buf = (char*)GlobalLock(hclip);//对内存块加锁,并返回它的内存地址
		strcpy_s(buf, size, data.c_str());        //将字符拷贝到内存中
		GlobalUnlock(hclip);     //解锁
		SetClipboardData(CF_TEXT, hclip);//向剪切板上放置内存中的数据
		CloseClipboard();        //关闭剪切板
	}
}

void clipboardFormat(void) {
	
	if (OpenClipboard(NULL) )    //打开剪切板
	{
		HGLOBAL hGlobal = NULL;
		hGlobal = ::GetClipboardData(CF_UNICODETEXT);
		if (hGlobal == NULL) {
			return ;
		}
		//string str = wtoc(pGlobal);
		wstring wsGlobal = (wchar_t *)GlobalLock(hGlobal);
		CloseClipboard();        //关闭剪切板



		wstring wsDelim = L"\r\n";
		std::vector<wstring>  lines = ws_split(wsGlobal, wsDelim);

		// 遍历过滤
		//string::size_type idx;

		std::vector<wstring> allData;
		std::map<wstring, std::vector<wstring>> userRecord;
		std::vector<wstring>::iterator itAll;
		int length = lines.size();
		for (size_t i = 0; i < length; i++)
		{
			if (length <= i)
				break;
			if (lines.at(i).find(L"撤回了一条消息") != wstring::npos
				|| lines.at(i).find(L"退出了群组") != wstring::npos
				|| lines.at(i).find(L"加入了群组") != wstring::npos) {
				continue;
			}
			if (lines.at(i).find(L":") != string::npos) {
				wstring user = lines.at(i);
				if (i + 1 < lines.size()) {
					wstring next = lines.at(i + 1);
					wchar_t code = next.back();
					//if (next == "[图片]") {
					//	//跳过
					//	i++;
					//	continue;
					//}
					if (code == L':') {
						continue;
					}

					if (next == L"无效" || next == L"无") {
						if (0 < userRecord[user].size()) {
							// 删除上有一条记录 (用户名+一次聊天内容 == 一条记录)
							wstring last = userRecord[user].back();
							for (itAll = allData.begin(); itAll != allData.end(); itAll++)
							{
								if (*itAll == user + L"\r\n") {
									itAll = allData.erase(itAll);
									break;
								}
							}
							for (itAll = allData.begin(); itAll != allData.end(); itAll++)
							{
								if (*itAll == last + L"\r\n") {
									itAll = allData.erase(itAll);
									break;
								}
							}
						}
					}
					else {
						userRecord[user].push_back(next);
						allData.push_back(user + L"\r\n");
						allData.push_back(next + L"\r\n");
						// 跳至下一条记录
					}
					i++;
					continue;
				}
			}
			allData.push_back(lines.at(i) + L"\r\n");
		}
		wstring str;
		for (itAll = allData.begin(); itAll != allData.end(); itAll++)
		{
			str.append(*itAll);
		}
		allData.clear();






		//将Text数据复制到Global分配的数据区
		hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (lstrlen(str.c_str()) + 1) * sizeof(TCHAR));
		LPWSTR pGlobal = (LPWSTR)GlobalLock(hGlobal);
		lstrcpy(pGlobal, str.c_str());
		GlobalUnlock(hGlobal);

		//将Global数据设为剪切板所有以实现跨进程使用
		OpenClipboard(NULL);
		EmptyClipboard();
		isLocal = !isLocal;
		SetClipboardData(CF_UNICODETEXT, hGlobal);
		CloseClipboard();
	}
}



//运行程序或路径
void execute(std::string s) {
	ShellExecuteA(
		NULL,
		("open"),
		("Explorer.exe"),
		(s.c_str()),
		NULL,
		SW_SHOWDEFAULT);
}

// 获取使用时长
int GetUseTime(LPCTSTR lpFilePath)
{
	HANDLE hFile;
	//打开文件
	hFile = CreateFile(lpFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		show(NULL, "授权文件缺失,请您联系客服!");
		exit(0);
		return -1;
	}
	LARGE_INTEGER size;
	::GetFileSizeEx(hFile, &size);
	__int64 nSize1 = size.QuadPart;
	if (nSize1 == 0 || 5<nSize1) {
		show(NULL, "授权文件数据丢失,请您联系客服!");
		exit(0);
		return -1;
	}

	//接收实际读取到的数据
	unsigned char ch[100];
	memset(ch,0,sizeof(ch));
	//接收实际读取到的字节数
	DWORD dwReads;
	//读取数据
	ReadFile(hFile, ch, 100, &dwReads, NULL);
	//设置字符串结束字符
	ch[dwReads] = 0;
	//关闭打开的文件对象的句柄
	CloseHandle(hFile);
	//显示读取到的数据
	for (int j = 0; j < 4; j++) {
		ch[j] = ch[j] ^ (2 * j + 1);
	}
	int value = 0;
	// 由高位到低位  
	for (int i = 0; i < 4; i++) {
		int shift = (4 - 1 - i) * 8;
		value += (ch[i] & 0x000000FF) << shift;// 往高位游  
	}
	if (365*24*60*60 < value) {
		show(NULL, "授权文件数据错误,请您联系客服!");
		exit(0);
		return -1;
	}
	//char szShow[64];
	//sprintf_s(szShow,64,"展示%d值",value);
	//show(NULL, szShow);
	return value;
}

// 写入时长
void SetUseTime(LPCTSTR lpFilePath,int i)
{
	//定义一个句柄变量
	HANDLE hFile;
	//创建文件
	hFile = CreateFile(lpFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;
	//接收实际写入的字节数
	DWORD dwWrites = 0;
	//写入数据
	unsigned char result[4];
	result[0] = (unsigned char)((i >> 24) & 0xFF);
	result[1] = (unsigned char)((i >> 16) & 0xFF);
	result[2] = (unsigned char)((i >> 8) & 0xFF);
	result[3] = (unsigned char)(i & 0xFF);
	for (int j = 0; j < 4;j++) {
		result[j] = result[j] ^ (2*j+1);
	}

	WriteFile(hFile, result, 4, &dwWrites, NULL);
	FlushFileBuffers(hFile);
	//关闭文件句柄
	CloseHandle(hFile);
}



/////////////////////////

#define MAX_LOADSTRING 100
#define MYKEY1 0x2901

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//定时器回调函数  
void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);

// 自定义
BOOL isOpen = false;
HWND hwndNextViewer;

UINT nFormat = (UINT)(-1);
static int remain = 0;

UINT auPriorityList[] = {
   CF_OWNERDISPLAY,
   CF_UNICODETEXT,
   CF_ENHMETAFILE,
   CF_BITMAP
};

////////////定时器///////////
//定时事件  
UINT_PTR dwTimerId = 0;

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (dwTimerId == idEvent)
	{
		if (remain-- <= 0) {
			DestroyWindow(hwnd);
			KillTimer(NULL, dwTimerId);
			show(hwnd, "时间到了!");
			exit(0);
	
		}
	}
}

DWORD CALLBACK Thread(PVOID pvoid)
{
	MSG  msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	dwTimerId = SetTimer(NULL, 111, 1000, TimerProc);
	BOOL  bRet;

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			//handle the error and possibly exit  
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	KillTimer(NULL, dwTimerId);
	printf("thread   end   here\n");
	return   0;
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。
	// 如果时间不满足,则立即退出
	remain = GetUseTime(limitPath);
	if (remain <= 0) {
		show(NULL, "时间到了!");
		exit(0);
		return true;
	}
	else {
		char szHint[256];
		int day = remain / (60 * 60 * 24);
		int rHour = remain % (60 * 60 * 24);
		int hour = rHour / (60 * 60);
		int rMinue = rHour % (60 * 60);
		int minue = rMinue / 60;
		int second = rMinue % 60;
		sprintf_s(szHint, 256, "您当前可用的时长： %d天%d时%d分%d秒", day, hour, minue, second);
		show(NULL, szHint);
	}

	//设置定时器  
	DWORD dwThreadId = 0;
	printf("use timer in workthread of console application\n");
	HANDLE hThread = CreateThread(NULL, 0, Thread, 0, 0, &dwThreadId);

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FXFORMAT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FXFORMAT));
	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_HOTKEY)//热键消息
		{

			//show(NULL, str);
		}
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FXFORMAT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FXFORMAT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}
	//wchar_t  runPath[MAX_PATH];
	//GetModuleFileName(NULL, runPath, MAX_PATH);


	::SetWindowPos(hWnd, nullptr, 550, 350, 300, 250, WS_MAXIMIZEBOX);
	ShowWindow(hWnd, nCmdShow);

	// 这段代码将当前窗口带到带到最顶层，并设置为活动窗口
	//HWND hForegdWnd = ::GetForegroundWindow();
	//DWORD dwCurID = ::GetCurrentThreadId();
	//DWORD dwForeID = ::GetWindowThreadProcessId(hForegdWnd, NULL);
	//::AttachThreadInput(dwCurID, dwForeID, TRUE);
	//::SetForegroundWindow(hWnd);
	//::AttachThreadInput(dwCurID, dwForeID, FALSE);


	UpdateWindow(hWnd);

	return TRUE;
}


// “关于”框的消息处理程序。
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








LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_OPEN_FORMAT://打开
			isOpen = true;
			show(NULL, "开启格式刷");
			break;
		case ID_CLOSE_FORMAT: //关闭
			isOpen = false;
			show(NULL, "格式刷已关闭");
			break;
		}
	}
	break;
	case WM_CREATE:
		//将本窗口注册到Clipboard Viewer Chain，
		//并保存Clipboard Viewer Chain中下一个窗口的句柄
		hwndNextViewer = SetClipboardViewer(hWnd);
		break;
	case WM_DRAWCLIPBOARD: // 粘贴板的内容改变时会收到此消息
	{
		if (isOpen) {
			nFormat = GetPriorityClipboardFormat(auPriorityList, sizeof(auPriorityList));
			if (nFormat == CF_UNICODETEXT)
			{
				if (isLocal) {
					isLocal = !isLocal;
					return false;
				}
				clipboardFormat();
			}
			else if (nFormat == CF_BITMAP)
			{

			}
			SendMessage(hwndNextViewer, message, wParam, lParam);
		}
	}

	break;
	case WM_CHANGECBCHAIN: //Clipboard Viewer注销

	   //如果注销的Clipboard Viewer窗口是本窗口的下一个窗口，

	   //则修改本窗口保存的下一个窗口句柄,

	   //否则将该消息传递到Clipboard Viewer Chain的下一个窗口

		if ((HWND)wParam == hwndNextViewer)

			hwndNextViewer = (HWND)lParam;

		else if (hwndNextViewer != NULL)

			SendMessage(hwndNextViewer, message, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:

		break;

	case WM_RBUTTONDOWN:

		break;

	case WM_CLOSE:

		DestroyWindow(hWnd); //既关闭按钮,可以在此时询问用户是否确定要关闭,再调用DestroyWindow关闭程序

		break;

	case WM_DESTROY:

		//从Clipboard Viewer Chain注销本窗口
		// 记录使用时长
		SetUseTime(limitPath, remain);
		ChangeClipboardChain(hWnd, hwndNextViewer);

		PostQuitMessage(0);

		break;

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);

	}

	return 0;

}