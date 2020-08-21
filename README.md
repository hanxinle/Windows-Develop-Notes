#  Windows 程序设计


> For: windows 10 + Visual Studio 2017. VC++6.0 not recommend. 


## MFC 入门示例(4个)

* add  

基本对话框程序，演示如何使用 Edit 控件，Button 控件及MFC自带的 CString 类。
 
* MFC 中使用控制台

添加 consoleInMfc.cpp to the MFC (Dialog base) 项目.在 xxDlg.cpp 文件及所在项目中进行如下操作：
    
1. add "# include <istream> ";   
2. and a button.in function on button add "cout<<""<<endl;";   
3. set Project-Setting-Link-sbusystem:windows as subsystem:console;   

执行程序。

* CFile 

CString 类型存储文件名、写入文件内容，储存读取的文件内容（PS： 该实现有 bug）。

* RegEdit

注册表修改程序，能够打开注册表，查看根键、子键的值。创建新的子键并且设置它的值，也可以删除子键及子键的值。

<center>
MFC 中的数据类型


|    |     |
|:--:|:---:|
| BOOL     |      TRUE | FALSE
| BSTR     |      32 BIT CHAR POINTER
| BYTE     |      8 BIT ,likes unsigned char in c
| COLORREF |      32BIT DESCRIBE COLOR,USED IN GDI
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
| POSITION |     POSITION OF ELEMENT IN SET
| LPCRECT  |     32BIT POITER TO A CONST RECT STRUCT

</center>

> PS: 最后两个是 MFC　独有的。
