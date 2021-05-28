@echo off
set CONAN_LOCAL=1

for /f %%i in ('type version') do set VERS=%%i


conan create . edwardstock/latest -s build_type=Debug --build=missing
conan create . edwardstock/latest -s build_type=Release --build=missing

@rem conan upload bigmath/%VERS%@edwardstock/latest --all -r=edwardstock