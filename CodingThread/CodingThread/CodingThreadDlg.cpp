#include "CodingThreadDlg.h"


CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;



CodingThreadDlg::CodingThreadDlg(void)
{
	ptr = this;
}

CodingThreadDlg::~CodingThreadDlg(void)
{
	
}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
	ReleaseMutex(hMutex); // освобождения мьютекса
	EndDialog(hwnd, 0);

}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
	CodingThreadDlg *ptr = (CodingThreadDlg *)lp;
	char buf[100];
	ifstream in(TEXT("music.txt"),ios::in);
	if(!in)
	{
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));

	DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

		ofstream out("coding.txt", ios::out | ios::trunc);
		const int KEY = 100;
		while(!in.eof())
		{
			in.getline(buf,100);
			out << buf << endl;
		}
		out.close();
		in.close();
		ReleaseMutex(hMutex); // освобождения мьютекса

		MessageBox(ptr->hDialog, TEXT("Запись данных в файл coding.txt завершена!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);


	return 0;
}

DWORD WINAPI Write_File_Thread(LPVOID lp)
{
	CodingThreadDlg* ptr = (CodingThreadDlg*)lp;
	char buf[100];
	ifstream in(TEXT("music.txt"), ios::in);
	if (!in)
	{
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));

	DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);
	while (!in.eof())
	{
		wchar_t wstr[100];
		in.getline(buf, 100);
		MultiByteToWideChar(CP_UTF8, 0, buf, -1, wstr, 100);
		SendMessage(ptr->hEdit1, EM_REPLACESEL, TRUE, (LPARAM)wstr);
		SendMessage(ptr->hEdit1, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
	}
	ReleaseMutex(hMutex);
}

void CodingThreadDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON1)
	{
		DestroyWindow(hwnd);
	}
}

BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{

	hDialog = hwnd;
	hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
	CreateThread(NULL, 0, Coding_Thread, this, 0, NULL); 
	CreateThread(NULL, 0, Write_File_Thread, this, 0, NULL);
	return TRUE;
}


BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}