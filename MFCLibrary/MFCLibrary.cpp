// MFCLibrary.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//вTODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CMFCLibraryApp

struct Header
{
	int h_size;
	int h_thread;
};

HANDLE hRead, hWrite;
HANDLE hReadConfirm, hWriteConfirm;
HANDLE hPipe;

_declspec(dllexport) void _stdcall OpenPipe()
{
	hPipe = CreateNamedPipe("\\\\.\\pipe\\MyPipe",
		PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
		1024, 1024, 0, NULL);
}

_declspec(dllexport) inline void _stdcall Connect() {
	ConnectNamedPipe(hPipe, NULL);
}

_declspec(dllexport) inline void _stdcall ServerDisconnect() {
	DisconnectNamedPipe(hPipe);
}

extern "C"
{
	_declspec(dllexport) void _stdcall Disconnect()
	{
		CloseHandle(hPipe);
	}
	_declspec(dllexport) bool _stdcall ConnectToServer()
	{
		if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 5000))
		{
			hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			return true;
		}
		else return false;
	}

	_declspec(dllexport) inline int _stdcall GetInt()
	{
		DWORD dwDone;
		int n;

		ReadFile(hPipe, &n, sizeof(n), &dwDone, NULL);
		return n;
	}

	_declspec(dllexport) inline void _stdcall SendInt(int n)
	{
		DWORD dwDone;

		WriteFile(hPipe, &n, sizeof(n), &dwDone, NULL);
		FlushFileBuffers(hPipe);
	}

	_declspec(dllexport) inline char* _stdcall GetString(int& threadNumber)
	{
		DWORD dwDone;
		Header header;

		ReadFile(hPipe, &header, sizeof(Header), &dwDone, NULL);
		char* str = new char[header.h_size];
		threadNumber = header.h_thread;
		ReadFile(hPipe, str, header.h_size, &dwDone, NULL);
		return str;
	}

	_declspec(dllexport) inline void _stdcall SendString(char* str, int threadNumber)
	{
		DWORD dwDone;
		Header header = { strlen(str) + 1, threadNumber };

		WriteFile(hPipe, &header, sizeof(Header), &dwDone, NULL);
		WriteFile(hPipe, str, header.h_size, &dwDone, NULL);
		FlushFileBuffers(hPipe);
	}
	_declspec(dllexport) inline int _stdcall confirm() {
		while (true) {
			int response = GetInt();
			if (response != 0) {
				return response;
			}
		}
	}
}

void Cleanup()
{
	CloseHandle(hRead);
	CloseHandle(hWrite);
	CloseHandle(hReadConfirm);
	CloseHandle(hWriteConfirm);
}

BEGIN_MESSAGE_MAP(CMFCLibraryApp, CWinApp)
END_MESSAGE_MAP()


// Создание CMFCLibraryApp

CMFCLibraryApp::CMFCLibraryApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}

// Единственный объект CMFCLibraryApp

CMFCLibraryApp theApp;
BOOL APIENTRY MFCLibrary(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Cleanup();
		break;
	}
	return TRUE;
}

// Инициализация CMFCLibraryApp

BOOL CMFCLibraryApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}