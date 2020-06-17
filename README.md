# PSO2 SymbolArt Thumbnail Handler

Shell Extension for Windows Explorer to display Symbol Art file thumbnails and in the preview pane

Makes it straightforward to browse your cache and pick which ones you want to import from windows explorer itself

<details>
  <summary>Sample Screenshot</summary>

![image](https://user-images.githubusercontent.com/12700106/84848317-4d954f80-b029-11ea-8396-00715ba719b8.png)

</details>

# Requirements

* Windows
* Visual C++ 2019 Runtime (v142) <sup>[1](#msvcrt)</sup>
* A OpenGL 3.3 capable card

You can use the included SarConvert application to test if the thumbnail handler should work.

# Installation/Removal

* Grab the latest build from the releases page
* Extract the zip file somewhere it won't be moved out of (like alongside your Tweaker/Game executable for instance)

Then either

* Run the provided install/remove script as administrator

Or

* Open a Command Prompt as administrator 
* CD into the folder with the dll file
* run `regsrv32 Pso2SarThumbnailHandler.x64.dll` or `regsrv32 Pso2SarThumbnailHandler.x86.dll` to install
* run `regsrv32 /u Pso2SarThumbnailHandler.x64.dll` or `regsrv32 /u Pso2SarThumbnailHandler.x86.dll` to remove


# SarConvert Utility

Included in the release download, you can use it to test if the thumbnail handler should work, or simply to convert .sar files into .pngs for general use

### Usage:

Droping one or more .sar files into the executable will create a 1:1 (usually 192x96, or 32x32) .png file of it, 
For precise size control, run it from a command prompt or script file

Passing a number as parameter will change the out size for the subsequent arguments

Examples:

`SarConvert.exe 512 input1.sar input2.sar ... inputN.sar` 
will create 512px .pngs

`SarConvert.exe 512 input1.sar 768 input2.sar 1024 input3.sar`
will create 512, 768, and 1024px pngs respectively

------

<a name="msvcrt">1</a>: Visual C++ 2019 Runtime: [x86](https://aka.ms/vs/16/release/vc_redist.x86.exe) / [x64](https://aka.ms/vs/16/release/vc_redist.x64.exe)
