import os

from conans import ConanFile, CMake, tools


def get_version():
    with open(os.path.join(os.path.dirname(__file__), 'version'), 'r') as f:
        content = f.read()
        try:
            content = content.decode()
        except AttributeError:
            pass
        return content.strip()


class BigmathConan(ConanFile):
    name = "bigmath"
    version = get_version()
    license = "MIT"
    author = "Eduard Maximovich edward.vstock@gmail.com"
    url = "https://github.com/edwardstock/bigmath"
    description = "Bigmath library to handle arbitrary floating point numbers and big integers. Uses GMP and mpdecimal"
    topics = ("cpp-helpers", "helpers")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {
        "shared": False,
        "gmp:shared": False,
        "mpdecimal:shared": False
    }
    generators = "cmake"
    exports = "version"
    exports_sources = (
        "cfg/*",
        "modules/*",
        "options.cmake",
        "include/*",
        "tests/*",
        "src/*",
        "CMakeLists.txt",
        "conanfile.py",
        "LICENSE",
        "README.md"
    )
    requires = (
        "mpir/3.0.0",
        "mpdecimal/2.5.1"
    )
    build_requires = (
        "gtest/1.10.0",
    )

    def source(self):
        if "CONAN_LOCAL" not in os.environ:
            git = tools.Git(folder="bigmath")
            git.clone("https://github.com/edwardstock/bigmath.git", "master")

    def configure(self):
        if self.settings.compiler == "Visual Studio":
            del self.settings.compiler.runtime
            del self.options.shared

    def build(self):
        cmake = CMake(self)
        opts = {
            'CMAKE_BUILD_TYPE': self.settings.build_type,
            'ENABLE_SHARED': "Off",
        }

        if self.settings.compiler != "Visual Studio" and self.options.shared:
            opts['ENABLE_SHARED'] = "On"

        cmake.configure(defs=opts)
        cmake.build(target="bigmath")

    def package(self):
        self.copy("*", dst="include", src="include", keep_path=True)
        dir_types = ['bin', 'lib', 'Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel']
        file_types = ['lib', 'dll', 'dll.a', 'a', 'so', 'exp', 'pdb', 'ilk', 'dylib']

        for dirname in dir_types:
            for ftype in file_types:
                self.copy("*." + ftype, src=dirname, dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["bigmath"]
        if self.settings.build_type == "Debug" and self.settings.compiler != "Visual Studio":
            self.cpp_info.cxxflags.append("-g")

    def test(self):
        cmake = CMake(self)
        cmake.configure([], {'ENABLE_TEST': 'On'})
        cmake.build([], None, "bigmath-test")
        self.run("bin/bigmath-test")
