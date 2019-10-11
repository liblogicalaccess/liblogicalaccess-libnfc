from conans import ConanFile, CMake, tools
import os

class LLANFCConan(ConanFile):
    name = "LogicalAccessNFC"
    version = "2.1.0"
    license = "https://github.com/islog/liblogicalaccess-libnfc/blob/master/LICENSE.LibLogicalAccess-LibNFC.txt"
    url = "https://github.com/islog/liblogicalaccess-libnfc"
    description = "LibLogicalAccess plugin to use NFC readers supported by LibNFC project"
    settings = "os", "compiler", "build_type", "arch"
    requires = 'LogicalAccess/' + version + '@islog/master', 'LibNFC/1.7.1@cis/stable'
    generators = "cmake"
    options = {}
    default_options = ''
    revision_mode = "scm"
    exports_sources = "CMakeLists.txt", "cmake*", "plugins*" 

    def configure_cmake(self):
        cmake = CMake(self)
        if tools.os_info.is_windows:
            # For MSVC we need to restrict configuration type to avoid issues.
            cmake.definitions['CMAKE_CONFIGURATION_TYPES'] = self.settings.build_type

        cmake.definitions['LIBLOGICALACCESS_VERSION_STRING'] = self.version
        cmake.definitions['LIBLOGICALACCESS_WINDOWS_VERSION'] = self.version.replace('.', ',') + ',0'
        cmake.definitions['TARGET_ARCH'] = self.settings.arch
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs.append('libnfc-nfcreaders')
