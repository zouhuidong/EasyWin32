# EasyX_Win32Ctrl
Let easyx lib supports win32 ctrl. 让EasyX库支持Win32控件。

---

  EasyX 在2020.12.6之前一直不支持Win32控件（据说明年会支持？），我就写了这样一个库来使使用EasyX的同时使用Win32控件。
  原理是将EasyX绘制的图像BitBlt到自己创建的win32程序上，这样Win32程序就和EasyX共存了。

![image](https://github.com/zouhuidong/EasyX_Win32Ctrl/blob/main/screenshot/scrshot.jpg)
