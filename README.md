# custom-chrome

A repository dedicated to demonstrating how to build high-DPI aware Win32 GUIs with content protruding into the non-client area on Windows 10 (down to Vista where DWM was introduced), while preserving the user's Windows theme settings, doing the bare minimum in handling window behavior.

![](http://i.imgur.com/UKcKYot.png)

You can find a short recap on how it works [here](https://github.com/oberth/custom-chrome/wiki). This was developed for demonstration purposes at the behest of a friend who brought it up as a curiosity, with minimal time invested being the driving principle meaning the **code is quick and dirty**. It doesn't need further development, hence there will be no further development, maybe some feature here and there. It is licensed under MIT, as Unlicense has issues in certain countries. Enjoy.

## What this sample *isn't* about

1. Code that you're supposed to drop into your code **(it is for demonstration purposes only)**.
2. Building a framework you can use for your applications.
3. Implementing any logic behind the pretty face of it, mockup is just to show the potential.

![](http://i.imgur.com/DEdwte2.jpg)

This approach is heavily undocumented, especially as it pertains to preserving the style and behavior of an ordinary window. There is also a lot of misunderstanding in the community on how to go about doing these feats, especially as they were made popular by the distinct visual style of browsers like Mozilla Firefox (which redraws it personally) and Google Chrome which place their tabs into the window frame, along with some less obvious examples like Ribbon UI based interfaces (Windows/File Explorer).
