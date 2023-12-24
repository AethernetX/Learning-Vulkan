# Learning vulkan

this repos is being used for me to learn vulkan as well as rely on a text editor instead of using an IDE

the environment is being worked on in Windows and using mingw makefiles. the compiler being used is g++

# how to build

if for some odd reason you want to build the project you need to do the following:

1. make a file called `.env.cmake`

the content of the file should include the following

```

set(GLFW_PATH "[path to the GLFW libraries]")
set(GLM_PATH "[path to the glm folder]")
set(VULKAN_SDK_PATH  "[path to the vulkan sdk]")

set(MINGW_PATH "[path to the binaries for mingw32-make and g++]")

```

2. open up the folder in vscode and do `ctrl + shift + b`
3. in theory you should have a window