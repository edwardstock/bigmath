# BigMath

| Bintray | Windows | Linux & macOS |
|:--------:|:---------:|:-----------------:|
|[ ![Download](https://api.bintray.com/packages/edwardstock/conan-public/bigmath%3Aconan-public/images/download.svg) ](https://bintray.com/edwardstock/conan-public/bigmath%3Aconan-public/_latestVersion)|unavailable|[![CircleCI](https://circleci.com/gh/edwardstock/bigmath/tree/master.svg?style=svg)](https://circleci.com/gh/edwardstock/bigmath/tree/master)|



## Reqs
* cmake >= 3.9
* gnu gcc 5+/clang 5+/msvc
* make
* GMP or MPIR
* [mpdecimal](https://www.bytereef.org/mpdecimal/)

## Usage and features

Namespace for all things is bigmath

Arbitrary precision decimal numbers:
```c++
#include <bigmath/bigdecimal.h>
#include <bigmath/bigint.h>

void do_something() {
    bigmath::bigdecimal val("124.450200000000000200");
    bigmath::bigdecimal base("1000000000000000000");
    bigmath::bigdecimal res = val * base;
    bigmath::bigint = res.to_bigint();
    
    // just output as-is
    std::cout << res << std::endl;
    // output with only 6 digits after point (format string has Python syntax)
    std::cout << res.format(".6f") << std::endl;
    // convert decimal to bigint (drops everything after point)
    std::cout << res.to_bigint() << std::endl;
}
```

# Add to project
## Using Conan

Add remote:
```bash
conan remote add edwardstock https://api.bintray.com/conan/edwardstock/conan-public
```

```bash
conan install <path/to/your/project> bigmath/1.0.0@edwardstock/latest
```

or using `conanfile.txt`
```ini
[requires]
bigmath/1.0.0@edwardstock/latest

[generators]
cmake
```

## Include to your CMake project
* Add submodule or just clone
  * `git submodule add https://github.com/edwardstock/bigmath.git /path/to/libs/bigmath` 
  * `git clone https://github.com/edwardstock/bigmath.git /path/to/libs/bigmath`
  
* Edit `CMakeLists.txt`:
```cmake
# your executable or library
add_executable(my_executable) 
#or if library
add_library(my_library)

# include module
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/path/to/libs/bigmath)

# link with your project
target_link_libraries(my_[executable|library] bigmath)
```

## Global Install
This library requires to be built

If you have conan, then just build, dependencies automatically will be installed and build (if required)
```bash
git clone https://github.com/edwardstock/bigmath.git bigmath && mkdir -p build && cd bigmath/build
cmake --build . --target install
```

If you DON'T have conan, then you have to install dependencies by yourself:
- libgmp or libmpir
- mpdecimal (source link is on the top of readme)

```bash
git clone https://github.com/edwardstock/bigmath.git bigmath && mkdir -p build && cd bigmath/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CONAN=Off
cmake --build . --target install
```

## Testing
```bash
git clone https://github.com/edwardstock/bigmath.git bigmath && mkdir -p build && cd bigmath/build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=On
cmake --build . --target bigmath-test
./bin/bigmath-test
```