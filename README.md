#  windows-dev

----
### Env:windows 10 + Visual Studio 2017. Sometimes windows XP and vc6.0. 

---

 
## add 
Dialog base. Show how to use EDIT Control , Button and CString.
 
## use console in MFC 
add consoleInMfc.cpp to the MFC (Dialog base) project.In the xxxDlg.cpp file:    
1\ add "# include <istream.> ";   
2\ and a button.in function on button add "cout<<""<<endl;";   
3\ set Project-Setting-Link-sbusystem:windows as subsystem:console;   
OK,run !   
(PS:VC6.0 + windows10 or WindowsXP,VS2017 is bad.)

## CFile
CString store file name,write,read.CFile object ops 1.txt including write and read.


