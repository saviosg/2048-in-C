@echo off
setlocal EnableDelayedExpansion
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set os_arch=32 || set os_arch=64

IF %os_arch% == 64 (
	SET arch=x86_64
	SET setup_arch=!arch!
) ELSE (
	SET arch=i686
	SET setup_arch=x86
)

SET HOME=%cd%/home

IF NOT EXIST .cygwin_lock (
	utils\setup-%setup_arch%.exe -B -f -q -l %cd%\cygwin-local -R %cd%\cygwin-root -P mingw64-%arch%-SDL,mingw64-%arch%-SDL_ttf,mingw64-%arch%-gcc-core,mingw64-%arch%-binutils,make
	copy nul .cygwin_lock
)

IF NOT EXIST PDCursesMod (
	git clone --depth=1 https://github.com/Bill-Gray/PDCursesMod
)

SET prefix=%arch%-w64-mingw32-

cygwin-root\Cygwin.bat
PAUSE