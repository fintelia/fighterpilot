//
//#include <string>
//#include <windows.h>
//using namespace std;
//#include "load_save.h"
//
//wchar_t currentFileName[MAX_PATH*2]=L"";
//
//wchar_t charToWchar(char c)
//{
//	return (wchar_t)c;
//}
//char wcharToChar(wchar_t c)
//{
//	return (char)c; 
//}
//void charToWchar(char *c,wchar_t* target)
//{
//	int s=strlen(c);
////	wchar_t* w=(wchar_t*)malloc(s);
//	for(int i=0;i<s;i++)
//	{
//		target[i]=charToWchar(c[i]);
//	}
////	return w;
//}
//string openfilename(char *filter, HWND owner) {
//	OPENFILENAME ofn;
//	char fileName[MAX_PATH*2]="";
//	ZeroMemory(&ofn, sizeof(ofn));
//
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.hwndOwner = owner;
//	ofn.lpstrFilter = (LPCSTR)filter;
//	ofn.lpstrFile = (LPSTR)fileName;
//	ofn.nMaxFile = MAX_PATH;
//	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
//	ofn.lpstrDefExt = (LPCSTR)"";
//
//	string fileNameStr;
//
//	if ( GetOpenFileName(&ofn) )
//	{
//		memcpy(currentFileName,fileName,MAX_PATH*2);//remember the file name
//		for(int i=0;i<MAX_PATH*2;i++)
//			fileNameStr.append(1,fileName[i]);
//	}
//	return fileNameStr;
//}
//string savefilename(char *filter, HWND owner,char *ext,char *defaultFile) {
//	OPENFILENAME ofn;
//	//wchar_t fileName[MAX_PATH*2] = L"";
//	//memcpy(fileName,currentFileName,MAX_PATH*2);
//
//	ZeroMemory(&ofn, sizeof(ofn));
//
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.hwndOwner = owner;
//	ofn.lpstrFilter = (LPCSTR)filter;
//	ofn.lpstrFile = (LPSTR)currentFileName;//////
//	ofn.nMaxFile = MAX_PATH;
//	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
//	ofn.lpstrDefExt = (LPCSTR)"";
//	ofn.lpstrDefExt = ext;
//	string fileNameStr;
//
//	if ( GetSaveFileName(&ofn) )
//	{
//		for(int i=0;i<MAX_PATH*2;i++)
//			fileNameStr.append(1,(char)currentFileName[i]);
//	}
//	//cout << endl << "error code: " << CommDlgExtendedError() << endl;
//	//cout << fileNameStr;
//	return fileNameStr;
//}
