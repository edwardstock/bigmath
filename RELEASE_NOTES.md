# Changelog

## 1.0.8

- Using GMP instead of MPIR for Apple arm64 processor (M1)

## 1.0.7

- `mpd_qset_string` works wrong if string is integer number without decimal point. Fixed.

## 1.0.6

- Suppressed unknown bug assert while moving value from one bigdecimal to another
- Updated deploy script

## 1.0.5

- Updated decimal using mpdec++ source
- Fixed memory leak while converting bigdecimal to string
- Updated gtest to 10

## 1.0.4

- Automatically increase precision (if neccessary) while using operators `*=`, `+=` and `/=`

## 1.0.3

- Updated mpdecimal conan dependency to 2.5.1
- Fixed `_MSC_VER` redefinition

## 1.0.2 (Fri Jan 22 2021)

- Updated mpdecimal conan dependency to 2.5.0 (as 2.4.2 no more compiles on macOS Big Sur)

## 1.0.1 (Thu Jan 14 2021)

- Fixed package_info in conanfile.py
- Added test for bigint binary shifting

## 1.0.0 (Thu Sep 24 2020)

- Initial version
