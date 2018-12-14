// allfile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

//创建分解文件后，运行各分解文件时的进程
void Create_Process(const char* temp_exe, BOOL async)
{
     HANDLE hProcess;
     HANDLE hThread;
     PROCESS_INFORMATION PI;
     STARTUPINFO SI;

     memset(&SI, 0, sizeof(SI));
     SI.cb = sizeof(SI);
	 CreateProcess(temp_exe, NULL, NULL, NULL, FALSE,NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI);	 
	 hProcess = PI.hProcess;       
     hThread = PI.hThread;
     if (!async)  //同步执行
     {
          WaitForSingleObject(hProcess, INFINITE);
          unlink(temp_exe);
     }
}

//Extracting the data & Saving the data to a file 
BOOL ExtractFile(int number,const char* exname) 
{ 
	BOOL bret = FALSE;
    HMODULE hLibrary; 
    HRSRC hResource; 
    HGLOBAL hResourceLoaded; 
    LPBYTE lpBuffer; 

	char szFileFullPath[MAX_PATH],szProcessName[MAX_PATH]; 
	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH);//获取文件路径
     
    hLibrary = LoadLibrary(szFileFullPath); 
    if (NULL != hLibrary) 
    { 
        char* p = MAKEINTRESOURCE(number); 
        hResource = FindResource(hLibrary, MAKEINTRESOURCE(number), RT_RCDATA); 

        LPVOID   lpMsgBuf;    
        FormatMessage(      
              FORMAT_MESSAGE_ALLOCATE_BUFFER   |      
              FORMAT_MESSAGE_FROM_SYSTEM   |      
              FORMAT_MESSAGE_IGNORE_INSERTS,    
              NULL,    
              GetLastError(),   //获得错误代码 
              MAKELANGID(LANG_NEUTRAL,   SUBLANG_DEFAULT),   //   Default   language    
              (LPTSTR)   &lpMsgBuf,   //错误代码对应的文字描述 
              0,    
              NULL      
        );    
        //   Process   any   inserts   in   lpMsgBuf.    
        //   ...    
        //   Display   the   string.    
       // MessageBox(   NULL,   (LPCTSTR)lpMsgBuf,   "Error",   MB_OK   |   MB_ICONINFORMATION   );    
        //   Free   the   buffer.    
        LocalFree(   lpMsgBuf   );   

        if (NULL != hResource) 
                           { 
            hResourceLoaded = LoadResource(hLibrary, hResource); 
            if (NULL != hResourceLoaded)         
            { 
                lpBuffer = (LPBYTE) LockResource(hResourceLoaded);             
                if (NULL != lpBuffer)             
                {                 
                    // do something with lpBuffer here 
                    DWORD dwFileSize,dwBytesWritten; 
                    HANDLE hFile; 

                    dwFileSize = SizeofResource(hLibrary, hResource); 

                    hFile = CreateFile(exname, 
                               GENERIC_WRITE, 
                               0, 
                               NULL, 
                               CREATE_ALWAYS, 
                               FILE_ATTRIBUTE_NORMAL, 
                               NULL); 

                    if (INVALID_HANDLE_VALUE != hFile) 
                    { 
                        WriteFile(hFile, lpBuffer, dwFileSize, &dwBytesWritten, NULL); 
                        CloseHandle(hFile); 
						bret = TRUE;
                    } 
                } 
            }     
        } 
        FreeLibrary(hLibrary); 
    } 


	return bret;
} 

int main(int argc, char* argv[]) 
{
	char onefile[1024] = "excat1.exe";
	char twofile[1024] = "excat2.exe";

	if( !ExtractFile(104,onefile) )
	{
		printf("release excat1.exe error!\n");
	}
	else
	{
		printf("run excat1.exe ok!\n");
		Create_Process(onefile,1);
	}
	if( !ExtractFile(105,twofile) )
	{
		printf("release excat2.exe error!\n");
	}
	else
	{
		printf("run excat2.exe ok!\n");
		Create_Process(twofile,1);
	}
	return 0;
}

