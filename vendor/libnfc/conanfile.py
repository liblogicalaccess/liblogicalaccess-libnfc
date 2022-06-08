from conans import ConanFile, tools, CMake


class LibNFCConan(ConanFile):
    name = "LibNFC"
    version = "1.7.1"
    settings = "os", "compiler", "build_type", "arch"
    description = "libnfc"
    url = "None"
    license = "None"
    options = {'shared': [True]}
    default_options = 'shared=True'
    exports_sources = "linux*"
    
    def configure_cmake(self):
        cmake = CMake(self, build_type=self.settings.build_type)
        cmake.configure(source_folder='linux/libnfc-1.7.1')
        return cmake
    
    def build(self):
        if self.settings.os == 'Windows':
            # just use pre-package binaries
            return
        cmake = self.configure_cmake()
        cmake.build()
    
    def package(self):
        # Depending on target architecture we either package one or the other 
        # dlls
        if self.settings.os == 'Windows':
            if self.settings.arch == 'x86_64':
                self.copy('*.dll', 'bin', 'win/dll/x64/{}'.format(self.settings.build_type))
                self.copy('*.lib', 'lib', 'win/lib/x64')
                self.copy("libusb0.dll", 'bin', 'libusb_x64')
            else:
                self.copy('*.dll', 'bin', 'win/dll/Win32/{}'.format(self.settings.build_type))
                self.copy('*.lib', 'lib', 'win/lib/Win32')
                self.copy("libusb0.dll", 'bin', 'libusb_x86')
            self.copy('*.h', 'include', 'win/include')
        else:
            cmake = self.configure_cmake()
            cmake.install()
            
    def package_info(self):
        if self.settings.os == 'Windows':
            self.cpp_info.bindirs = ['bin']
            self.cpp_info.libdirs = ['lib']
            self.cpp_info.libs.append('libnfc.lib')
        else:
            self.cpp_info.libs.append("nfc")
