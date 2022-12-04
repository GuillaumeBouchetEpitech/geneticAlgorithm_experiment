
# Shared Framework

## How to Build

### Build native C++ libraries

```bash
# slow on modest internet connection
sh sh_install_thirdparties.sh

sh sh_build.sh
# this script is interactive:
# -> select platform
#   -> "native"
# -> then mode
#   -> "release"
# -> then rebuild
#   -> "yes"
```

Files expected to be created:
* ./lib/native/
  * lib_bullet_collision.a
  * lib_bullet_dynamics.a
  * lib_bullet_linearmath.a
  * lib_framework_graphic.a
  * lib_framework_physic.a
  * lib_framework_system.a

### Build webassembly (wasm) C++ libraries

```bash
# need emsdk 3.1.26
# => check: sh_install_tools.sh

# slow on modest internet connection
sh sh_install_thirdparties.sh

sh sh_build.sh
# this script is interactive:
# -> select platform
#   -> "web wasm"
# -> then mode
#   -> "release"
# -> then rebuild
#   -> "yes"
```

Files expected to be created:
* ./lib/web/
  * lib_bullet_collision.bc
  * lib_bullet_dynamics.bc
  * lib_bullet_linearmath.bc
  * lib_framework_graphic.bc
  * lib_framework_physic.bc
  * lib_framework_system.bc


