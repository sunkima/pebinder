// pebinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include <Windows.h>

BOOL ReplaceICO(LPCTSTR lpszApp, LPCTSTR lpszICO)
{
	if( NULL == lpszApp )
		return 0;
	if( NULL == lpszICO )
		return 0;

    HANDLE    hICO;
    //打开硬盘上的图标文件
    if (!(hICO = ::CreateFile(lpszICO, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
    {
        return FALSE;
    }
    LONG    lOffset;    //资源在文件中的偏移量
    DWORD    dwSize;        //加载后资源的大小
    DWORD    dwReadByte;    //读取文件时实际读取大小，API强制要求。其实没用……
    LPVOID    lpRes;        //指向更新RT_ICON的资源
    LPVOID    lpGIco;        //指向更新RT_GROUP_ICON的资源
    CONST BYTE byGSize = 0x18;    //RT_GROUP_ICON资源的大小，为常数。
    //分配RT_GROUP_ICON资源的内存。
    if (!(lpGIco = new BYTE[byGSize]))  {::CloseHandle(hICO); return FALSE;}
    memset(lpGIco, 0, byGSize);//清零RT_GROUP_ICON资源内存
    ::ReadFile(hICO, lpGIco, 0x06, &dwReadByte, NULL);//从文件中读取RT_GROUP_ICON资源头
    ::ReadFile(hICO, (LPVOID)((BYTE*)lpGIco + 0x08), 0x0C, &dwReadByte, NULL);//从文件中读取RT_GROUP_ICON资源
    //获取ICO资源的大小。
    memcpy((void*)&dwSize, (void*)((BYTE*)lpGIco + 0x10), sizeof(DWORD));
    //获取ICO资源的偏移量。
    ::ReadFile(hICO, (void*)&lOffset, sizeof(LONG), &dwReadByte, NULL);
    //分配ICO资源的内存。
    if (!(lpRes = new BYTE[dwSize])) {delete[] lpGIco; ::CloseHandle(hICO); return FALSE;}
    //偏移文件指针
    ::SetFilePointer(hICO, lOffset, NULL, FILE_BEGIN);
    //读取图标资源到内存。
    ::ReadFile(hICO, lpRes, dwSize, &dwReadByte, NULL);
    ::CloseHandle(hICO);
    //打开要更改图标的可执行文件。
    HANDLE    hApp;
    if (NULL == (hApp = ::BeginUpdateResource(lpszApp, NULL)))
    {
        delete[] lpGIco;
        delete[] lpRes;
        return FALSE;
    }
    //标记更新资源
    ::UpdateResource(hApp, RT_GROUP_ICON, MAKEINTRESOURCE(1), 0, lpGIco, byGSize);
    ::UpdateResource(hApp, RT_ICON, MAKEINTRESOURCE(1), 0, lpRes, dwSize);
    //写入新资源
    int error=0;
    if (!::EndUpdateResource(hApp, FALSE))
    {
        delete[] lpGIco;
        delete[] lpRes;
        error=GetLastError();
        return FALSE;
    }
    delete[] lpGIco;
    delete[] lpRes;
    return TRUE;
}


BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName )  
{     
    HMODULE hInstance = ::GetModuleHandle(NULL);//得到自身实例句柄  
            
    HRSRC hResID = ::FindResource(hInstance,MAKEINTRESOURCE(dwResName),lpResType);//查找资源  
    HGLOBAL hRes = ::LoadResource(hInstance,hResID);//加载资源  
    LPVOID pRes = ::LockResource(hRes);//锁定资源  
            
    if (pRes == NULL)//锁定失败  
    {  
        return FALSE;  
    }  
    DWORD dwResSize = ::SizeofResource(hInstance,hResID);//得到待释放资源文件大小  
    HANDLE hResFile = CreateFile(lpFilePathName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);//创建文件  
            
    if (INVALID_HANDLE_VALUE == hResFile)  
    {  
        //TRACE("创建文件失败！");  
        return FALSE;  
    }  
        
    DWORD dwWritten = 0;//写入文件的大小     
    WriteFile(hResFile,pRes,dwResSize,&dwWritten,NULL);//写入文件  
    CloseHandle(hResFile);//关闭文件句柄  
            
    return (dwResSize == dwWritten);//若写入大小等于文件大小，返回成功，否则失败  
}


BOOL ImportFile(char* sname,int resno , char* lst_name) 
{ 
	BOOL Bret = FALSE;
    HANDLE hFile; 
    DWORD dwFileSize,       
          dwBytesRead; 
    LPBYTE lpBuffer; 
     
    hFile = CreateFile(sname, GENERIC_READ,  
                       0, 
                       NULL, 
                       OPEN_EXISTING, 
                       FILE_ATTRIBUTE_NORMAL, 
                       NULL); 

    if (INVALID_HANDLE_VALUE != hFile) 
    { 
        dwFileSize = GetFileSize(hFile, NULL); 

        lpBuffer = new BYTE[dwFileSize]; 

        if (ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL) != FALSE) 
        { 
            // do something with lpBuffer here //Updating the resource data 
            HANDLE hResource; 
            hResource = BeginUpdateResource(lst_name, FALSE); 
            if (NULL != hResource) 
            { 
                char* p = MAKEINTRESOURCE(resno); 
                if (UpdateResource(hResource,  
                    RT_RCDATA,  
                    MAKEINTRESOURCE(resno),  
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  //使用系统缺省语言 
                    (LPVOID) lpBuffer,  
                    dwFileSize) != FALSE) 
                { 
                    EndUpdateResource(hResource, FALSE); 
					Bret = TRUE;
					DWORD dwError=GetLastError();
					printf("ok\n");
                } 
            } 
        } 
        delete [] lpBuffer;           
        CloseHandle(hFile); 
    } 

	return Bret;
} 

void showinfo()
{
		printf("usage:\n");
		printf("pebinder.exe --file frist.exe secoend.exe new_file.exe\n");
		printf("pebinder.exe --ico  ico.ico new_file.exe\n");
}


void imfile(const char* one , const char* two , const char* three)
{
	if( NULL == one )
		return ;
	if( NULL == two )
		return ;
	if( NULL == three )
		return ;



		char onefile[1024] = {0};
		char twofile[1024] = {0};
		char lastfile[1024] = {0};
		memcpy(onefile,one,strlen(one));
		memcpy(twofile,two,strlen(two));
		memcpy(lastfile,three,strlen(three));

		/*
		char onefile[1024] = "d://test//PEiD_ch.exe";
		char twofile[1024] = "d://test//PEiD.exe";
		char lastfile[1024] = "d://test//last.exe";
		*/

		if( FreeResFile(IDR_RC_MYDATA1,"RC_MYDATA",lastfile) )
		{
			printf("relase lastfile success!!\n");

			if( ImportFile(onefile,104,lastfile) )
			{
				printf("frist file import success\n");
			}
			else
			{
				printf("frist file import error!!!\n");
			}

			if( ImportFile(twofile,105,lastfile) )
			{
				printf("secoend file import success\n");
			}
			else
			{
				printf("secoend file import error!!!\n");
			}
		}
		else
		{
			printf("relase lastfile error!!\n");
		}
}


int main(int argc, char** argv) 
{
	if( argc > 2 )
	{
		char str_Type[10] = {0x00};
		char szParam1[10] = {0x00};
		char szParam2[10] = {0x00};
		strcpy(szParam1,"--file");
		strcpy(szParam2,"--ico");
		strcpy(str_Type,argv[1]);

		if( 0 == memcmp(str_Type,szParam1 , strlen(szParam1)) )
		{
			imfile(argv[2],argv[3],argv[4]);
		}
		else if( 0 == memcmp(str_Type,szParam2 , strlen(szParam2)) )
		{ 
			char str_icon[1024] = {0};
			char str_app[1024] = {0};
			strcpy(str_app,argv[3]);
			strcpy(str_icon,argv[2]);

			ReplaceICO(str_app,str_icon);
		}
		else
		{
			showinfo();
		}
		
	}
	else
	{
		showinfo();
	}

	

    return 0; 
} 
