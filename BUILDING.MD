# Building

## Requirement

- CMake
- Python 3
- depot_tools (https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up). Add depot to path before Python

## Instruction

Pull every submodules

```
git submodule update --init --recursive
```

Pull dawn submodules

```
cd external/dawn
cp scripts/standalone.gclient .gclient
gclient sync
```

Windows

```
cd external\dawn
copy scripts\standalone.gclient .gclient
gclient sync
```
