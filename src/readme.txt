Syntax:
Call with ./Proj6 (input path) (output path) (output width) (output height) [number of threads]. If the number of threads is not included, it will default to the number of cores on the invoked machine.

Dependencies:
By default, only BMPs can be loaded since CImg keeps trying to use GraphicsMagick
to open anything else. Edit Graphics435.h to enable the other formats if you're
not running in Windows.

If you're under Windows and need .png/.jpg support, this is going to be painful; you need libjpeg, libpng, and zlib,
all of which have GnuWin32 installers. Point the project includes and libraries at "C:\Program Files (x86)\GnuWin32\include" and "C:\Program Files (x86)\GnuWin32\lib", and copy jpeg62.dll, libpng3.dll, libpng12.dll, librle3.dll and zlib1.dll from "C:\Program Files (x86)\GnuWin32\bin".