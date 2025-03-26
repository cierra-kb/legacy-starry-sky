# Starry Sky (Legacy)
A *very* old version of the 1.3 GDPS mod menu.
![in-game](https://github.com/user-attachments/assets/af015019-6c65-4fd5-a4f6-4711859b4289)

## Building
Build requirements:
- CMake
- Git
- Android NDK r16b

1. Clone the repo recursively
```
git clone https://github.com/cierra-kb/legacy-starry-sky.git --recursive
```
2. Download and Extract the NDK somewhere. Afterwards, save its path to ANDROID_NDK_ROOT
```
export ANDROID_NDK_ROOT=~/NDK/r16b
```
3. Run `make`

## Licensing
This project is licensed under the [Mozilla Public License 2.0](https://www.mozilla.org/en-US/MPL/2.0/).

## Attribution
Code contained within the [dobby wrapper](include/dobby_wrapper) directory has been modified from [gdps-android-public](https://github.com/qimiko/gdps-android-public), which is licensed under the [Mozilla Public License 2.0](https://www.mozilla.org/en-US/MPL/2.0/).
