## Building
```
   # configure
   /opt/devkitpro/portlibs/wiiu/bin/powerpc-eabi-cmake -S. -Bbuild-wiiu -DPORT=WiiU -DBUILD_SAMPLES=True -DCMAKE_BUILD_TYPE=Debug # -DBUILD_TESTS=True
   # build
   cmake --build build-wiiu
   # clean
   cmake --build build-wiiu --target clean
   # install
   cmake --install build-wiiu
   # uninstall
   xargs rm < build-wiiu/install_manifest.txt
   cat build-wiiu/install_manifest.txt | xargs -L1 dirname | xargs rmdir -p
```

## Credits
- The WebKit angle build system from [here](https://github.com/WebKit/WebKit/tree/00550091d9fb55fe62e3b7d0d029732a8f6ce04b/Source/ThirdParty/ANGLE)
- The [angle vcpkg port](https://github.com/microsoft/vcpkg/tree/master/ports/angle)
