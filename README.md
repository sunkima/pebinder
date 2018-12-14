# pebinder

usage:pebinder.exe [--file] frist.exe secoend.exe new_file.exe

c:\>pebinder.exe --file frist.exe secoend.exe new.exe
pebinder.exe --ico img.ico new_file.exe

ps:
调用时，格式如下：FreeResFile(IDR_DLL ,"RT_DLL","D:\\1.exe");
如果是建立的空项目，记得添加上头文件 #include "resource.h",不然识别不了资源ID:IDR_DLL

另外附上添加资源过程：

在解决方案资源管理器中，右击资源文件，选择添加-〉资源，在弹出来的添加资源对话框中，选择导入，然后切换到所有文件（*.*）后，选择需要导入的文件，按下打开按钮后，在自定义资源类型对话框中的资源类型一栏填入资源类型，比如上例中“RT_DLL”,注意：这里的“”也要填入，然后按确定即可。
