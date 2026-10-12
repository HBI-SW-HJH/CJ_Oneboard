# OspBridge.dll build notes

This NativeOsp project is self-contained.

Required layout:

- NativeOsp/sdk/lib
- NativeOsp/sdk/app/utils
- NativeOsp/adi_Eth10BaseT1s_cfg.c
- NativeOsp/ex_fioOsp.c
- NativeOsp/ilas_led.c
- NativeOsp/iseled.c

The CMake build intentionally uses only `NativeOsp/sdk`.
It does not fall back to `C:/Analog Devices/EdgeStudio ...`.
If `NativeOsp/sdk` is missing, CMake stops with an error.

Typical build:

```powershell
cd NativeOsp
$env:PATH = "C:\mingw64\bin;" + $env:PATH
cmake -S . -B cmake-build -G Ninja
cmake --build cmake-build -j 8
```

The output DLL is generated at:

- NativeOsp/build/OspBridge.dll

The C# project copies that DLL to the application output folder after build.
