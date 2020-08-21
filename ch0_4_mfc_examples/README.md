# 4个 MFC 入门示例

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