# This tool is no longer needed
**5 years passed and we finally have an [option](https://github.com/telegramdesktop/tdesktop/commit/6f760d513ea41b8017d4abd5f88e615fd5c0a10e) to disable taskbar flash.**  
Congratulations!

# Mutegram
A small utility to disable taskbar flashing of Telegram Desktop (**only for Windows**).
## The problem
Telegram Desktop is surely a great app.  
It is native cross-platform app built using Qt so it has a solid performance and is relatively lightweight (imagine the same app on Electron framework).
But it has one crucial disadvantage for me. **It flashes taskbar icon when there is a notification.**  

I set my taskbar to auto-hide so it does not take any space on my screen when it's not needed.  
Telegram flashes **every time** you have a notification (until you close it to tray but I want to be able to ALT-TAB into it).
But when some app flashes its icon, taskbar stays opened and the only way to make it hide back is to bring that app to foreground.  

![Problem](https://i.imgur.com/YQEkrb8.png)

Actually I think it's Windows problem too because it could have at least an option to change this behavior but still, in my opinion Telegram flashes *too much*.  
I also don't understand why Telegram Desktop developers refuse to add a toggle to disable that flashing, they have an opened issue for *three years*, seems like they just ignore it.
## The solution
The core of this problem is [FlashBounce](https://github.com/telegramdesktop/tdesktop/blob/master/Telegram/SourceFiles/platform/win/notifications_manager_win.cpp#L324 "FlashBounce") function and it's [caller](https://github.com/telegramdesktop/tdesktop/blob/master/Telegram/SourceFiles/window/notifications_manager.cpp#L246 "caller").  
As you can see, there is no way to disable that «flashbounce» without disabling notifications completely.  
Now you have two options: you can either build Telegram from sources by yourself OR you can byte-patch Telegram executable to get rid of this orange thing.  
I chose the second path because I don't really want to setup the whole build environment to compile Telegram (although it's somewhat easy, they provide step-by-step tutorial).
## Usage
Just download or compile this project, and place both EasyHook32.dll and winmm.dll next to Telegram.exe (default path is `%APPDATA%/Telegram Desktop`).  
Now `FlashWindowEx` call will do nothing.
## Details
It uses proxy DLL technique to inject into Telegram.exe process automatically on process startup.  
Then it detours `FlashWindowEx` from user32.dll to make it do nothing.  
## License
The MIT License

Copyright (c) 2018 mxs42

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
