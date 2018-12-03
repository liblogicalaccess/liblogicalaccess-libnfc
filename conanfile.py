from conans import ConanFile, CMake, tools
import os

class LLANFCConan(ConanFile):
    name = "LogicalAccessNFC"
    version = "2.1.0"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of LLA here>"
    settings = "os", "compiler", "build_type", "arch"
    requires = 'LogicalAccess/2.1.0@islog/' + tools.Git().get_branch(), 'LibNFC/1.7.1@cis/stable'
    generators = "cmake"
    options = {}
    default_options = ''
    
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
