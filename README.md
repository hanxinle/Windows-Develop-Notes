#  Windows程序设计


> For: windows 10 + Visual Studio 2017. VC++6.0 not recommend. 


## MFC工程初步

> 建立 MFC 初步印象

* add 

Dialog base. Show how to use EDIT Control , Button and CString.
 
* use console in MFC 

add consoleInMfc.cpp to the MFC (Dialog base) project.In the xxxDlg.cpp file:    
1. add "# include <istream> ";   
2. and a button.in function on button add "cout<<""<<endl;";   
3. set Project-Setting-Link-sbusystem:windows as subsystem:console;   
OK,run !      


* CFile

CString store file name,write,read.CFile object ops 1.txt including write and read.

* RegEdit

RegEdit could open registry.See rootkey,subkey values.Create new Subkey.Delete Subkey,Subkey value.
<center>
Data Types in MFC
</center>

|    |    |
|:--:|:--:|
| BOOL    |      TRUE | FALSE
| BSTR    |      32 BIT CHAR POINTER
| BYTE    |      8 BIT ,likes unsigned char in c
| COLORREF   |   32BIT DESCRIBE COLOR,USED IN GDI
| DWORD    |     32BIT UNSINGNED INT
| LONG     |     32BIT INT (+ -)
| LPARAM   |     32BIT ,OFFTEN USED FOR FUNCTION'S Parameters
| LPCSTR   |     POITER TO 32BIT CONST STR
| LPSTR    |     POITER TO 32BIT STR
| LPCTSTR  |     POITER TO 32BIT CONST STR,Used for Unicode and DBCS
| LPTSTR   |     POITER TO 32BIT STR,USED for Unicode and DBCS
| LPVOID   |     POITER TO ANY TYPE
| LRESULT  |     window process or 32bit value callback function returns
| UINT     |     32BIT UNSIGNED INT
| WNDPROC  |     32BIT POITER TO WINDOWS FUNCTION
| WORD     |     16BIT UNSIGNED INT
| WPARAM   |     32BIT VALUE IN WIN32,OFTEN AS FUNCTION'S PARAMETERS
| POSITION    | POSITION OF ELEMENT IN SET
| LPCRECT     | 32BIT POITER TO A CONST RECT STRUCT

> PS: last two only in MFC
