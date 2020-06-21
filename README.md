# PSO2 SymbolArt Thumbnail Handler

Shell Extension for Windows Explorer to display Symbol Art file thumbnails and in the preview pane

Makes it straightforward to browse your cache and pick which ones you want to import from windows explorer itself

Also a .Sar -> .Png converting utility, with optional high-resolution rendering

<details>
  <summary>Sample Screenshot</summary>

![image](https://user-images.githubusercontent.com/12700106/85083909-95db7b80-b1a9-11ea-9d21-f950beab3a27.png)

</details>

<details>
  <summary>Sample High-Res Rendering of the above screenshot</summary>
  
  ![image](https://user-images.githubusercontent.com/12700106/85217232-c9163a00-b364-11ea-936e-eac10637e53a.png)

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

# Troubleshooting

**Q.** Some .sar files don't display the thumbnails, or the thumbnails keep disappearing and appearing again when scrolling

**A.** Try moving the handler dll to somewhere in your system drive (i put mine in the *SEGA* folder under *My Documents*), when placed on my secondary drive, things didn't work as expected. Don't ask why.

----
**Q.** The thumbnails don't show at all.

**A.** Try dropping a .sar file on the SarConvert Utility, if you get a .png out of it (will be in the same folder as the .sar file), check if the thumbnail handler is properly registered by looking at your registry for the entries:

Key|Name|Type|Value
-|-|-|-
HKEY_CLASSES_ROOT/.sar/ShellEx/{e357fccd-a995-4576-b01f-234630154e96}|(Default)|REG_SZ|**{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}**
HKEY_CLASSES_ROOT/CLSID/**{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}**|(Default)|REG_SZ|PSO2 Symbolart Thumbnail Handler
HKEY_CLASSES_ROOT/CLSID/**{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}**/InProcServer32|(Default)|REG_SZ|\<Full Path to the Handler DLL\>
HKEY_CLASSES_ROOT/CLSID/**{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}**/InProcServer32|ThreadingModel|REG_SZ|Apartment


# SarConvert Utility

Included in the release download, you can use it to test if the thumbnail handler should work, or simply to convert .sar files into .pngs for general use

### Usage:

Droping one or more .sar files into the executable will create a 1:1 (usually 192x96, or 32x32) .png file of it, 
For precise size control, run it from a command prompt or script file

Passing a number as parameter will change the out size for the subsequent arguments, if this number is negative, high-resolution rendering will be done, see images above for an example.

Examples:

`SarConvert.exe 512 input1.sar input2.sar ... inputN.sar` 
will create 512px .pngs

`SarConvert.exe 512 input1.sar 768 input2.sar 1024 input3.sar`
will create 512, 768, and 1024px pngs respectively

`SarConvert.exe -512 input.sar 512 input.sar`
will create a hd and a non-hd 512px png respectively

------

<a name="msvcrt">1</a>: Visual C++ 2019 Runtime: [x86](https://aka.ms/vs/16/release/vc_redist.x86.exe) / [x64](https://aka.ms/vs/16/release/vc_redist.x64.exe)
