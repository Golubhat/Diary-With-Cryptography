# Diary Application

A C++ diary application built with **wxWidgets** and **CMake**, designed to be developed and built using **Visual Studio Code**.

## Requirements

Install the following before building:

- Visual Studio Code
- CMake 3.16 or later
- A C++17-compatible compiler
- wxWidgets
- VS Code **C/C++** extension
- VS Code **CMake Tools** extension

### Windows

The project can be built with either:

- **MSVC** (recommended for Windows)
- **MinGW/GCC**

### Linux / macOS

The project can be built with GCC or Clang, provided wxWidgets is installed and available to CMake.

---

# Opening the Project in VS Code

Open a terminal and navigate to the project directory:


cd path/to/diary_wxwidgets


Then run:


code .


You can also open the `diary_wxwidgets` folder directly using:

**File → Open Folder**

---

# Selecting the Compiler

The project uses CMake, so the compiler is selected through **CMake Tools**.

Open the Command Palette:


Ctrl + Shift + P


Select:


CMake: Select Kit


Then select the desired compiler.

## MSVC

On Windows, select an MSVC kit similar to:


Visual Studio Community 2022 Release - amd64


or an appropriate Visual Studio Build Tools MSVC kit.

If MSVC is not detected, open VS Code from a **Developer Command Prompt for Visual Studio**:


cd C:\path\to\diary_wxwidgets
code .


Then verify MSVC:


cl


If configured correctly, the Microsoft C/C++ compiler version will be displayed.

## MinGW/GCC

On Windows, select the appropriate MinGW/GCC kit.

Verify GCC with:


g++ --version


CMake Tools should then be able to detect the MinGW/GCC toolchain.

---

# Configuring the Project

After selecting the compiler:


Ctrl + Shift + P


Select:


CMake: Configure


CMake will create the `build/` directory automatically.

If the compiler or generator is changed later, use:


CMake: Delete Cache and Reconfigure


---

# Building the Application

### Using CMake Tools

Open:


Ctrl + Shift + P


and select:


CMake: Build


### Using the VS Code Build Task

Use:


Terminal → Run Build Task


or press:


Ctrl + Shift + B


This runs the configured CMake build task.

---

# Running the Application

After a successful build, use:


Ctrl + Shift + P


and select:


CMake: Run Without Debugging


You can also use the Run/Debug controls in VS Code.

With the Ninja configuration, the Windows executable is normally:


build/Diary.exe


The exact location can vary depending on the selected CMake generator and platform.

---

# Debugging

Open:


Run and Debug


Then select the appropriate configuration:


Diary (MSVC)


for MSVC, or:


Diary (MinGW/GCC)


for MinGW/GCC.

Breakpoints can be placed in the `.cpp` files before starting the debugger.

---

# wxWidgets

The application requires wxWidgets.

CMake locates wxWidgets using:


find_package(wxWidgets REQUIRED COMPONENTS core base)


If CMake reports that wxWidgets cannot be found, verify that wxWidgets is installed and that its CMake/package configuration is available to CMake.

The wxWidgets installation is not included in this project.

---

# Application Data

The application creates its required data files at runtime.

These may include:


settings.dat
Encryption Code.dat


as well as user and backup data managed by the application.

These runtime files should not normally be included in the source-code submission.

If the encryption-code file does not exist, the application asks the user whether it should be created. Choosing **No** exits the application.

---

# Building from the Command Line

From the project root:


cmake -S . -B build


Then:


cmake --build build


The executable will be generated inside the `build/` directory according to the selected CMake generator.

---

# Clean Build

If CMake configuration or compiler settings become inconsistent:

1. Open the Command Palette.
2. Select:


CMake: Delete Cache and Reconfigure


3. Select:


CMake: Build


Alternatively, delete the generated `build/` directory and configure the project again.

---

# Important Notes

- The project uses **C++17**.
- `DiaryApp.cpp` contains the wxWidgets application entry point through `wxIMPLEMENT_APP(DiaryApp)`.
- The project uses CMake so that it can be built with different C++ toolchains.
- Windows-specific configuration, such as the application manifest, is handled conditionally by CMake.
- The wxWidgets installation is not included in the project.
