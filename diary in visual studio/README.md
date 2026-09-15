# Diary Application — Visual Studio Build Guide

This guide explains how to build and run the **Diary** C++ wxWidgets application using **Visual Studio**, **without CMake**.

The project is built directly as a Visual Studio C++ project.

---

# Requirements

Install:

- Visual Studio
- **Desktop development with C++** workload
- MSVC C++ build tools
- Windows SDK
- wxWidgets

The project uses **C++17**.

> This guide assumes Windows and MSVC.

---

# Installing wxWidgets

Visual Studio needs access to a compiled wxWidgets installation.

Make sure the wxWidgets build matches your Visual Studio configuration.

For example, if the project is built as:


x64
Debug


the wxWidgets libraries used by the project must be compatible with the corresponding MSVC architecture/configuration.

Do not mix incompatible:

- x86 and x64 libraries
- Debug and Release libraries
- incompatible MSVC toolsets

---

# Opening the Project

If a Visual Studio solution already exists:

1. Start **Visual Studio**.
2. Select:


Open a project or solution


3. Select:


Diary.sln


Visual Studio will load the project.

---

# Creating the Visual Studio Project

If the project does not already have a `.sln` / `.vcxproj`:

1. Open Visual Studio.
2. Select:


Create a new project


3. Choose:


Empty Project


4. Name it:


Diary_Project


5. Create the project.

Then add the existing source files.

---

# Adding Source Files

In **Solution Explorer**:

Right-click the project:


Diary_Project


Select:


Add → Existing Item...


Add the `.cpp` and `.h` files from `src`.

The important source files include:


DiaryApp.cpp
DiaryApp.h

MainFrame.cpp
MainFrame.h

LoginFrame.cpp
LoginFrame.h

RegistrationFrame.cpp
RegistrationFrame.h

DashboardFrame.cpp
DashboardFrame.h

UserManager.cpp
UserManager.h

Encryption.cpp
Encryption.h

BackupManager.cpp
BackupManager.h


Add the Windows manifest if the project uses it:


Diary.manifest


---

# Important: DiaryApp.cpp Is the Entry Point

The wxWidgets entry point is in:


DiaryApp.cpp


It contains:


wxIMPLEMENT_APP(DiaryApp);


Therefore:

- Make sure `DiaryApp.cpp` is included in the Visual Studio project.

---

# Configuring the Platform

At the top of Visual Studio, select the desired platform.

For a modern 64-bit Windows system, use:


x64


For example:


Debug | x64


or:


Release | x64


If `x64` is not available:

1. Open:


Build → Configuration Manager


2. Under **Active solution platform**, select:


x64


3. Create it if necessary.

---

# Setting C++17

Right-click the **Diary_Project** project:


Properties


Go to:


Configuration Properties
    → C/C++
        → Language


Set:


C++ Language Standard


to:


ISO C++17 Standard (/std:c++17)


Apply the setting to:


All Configurations


and, if appropriate:


All Platforms


---

# Configuring wxWidgets Include Directories

Open:


Project Properties


Go to:


Configuration Properties
    → C/C++
        → General
            → Additional Include Directories


Add the wxWidgets include directories.

A typical installation may contain paths similar to:


C:\wxWidgets\include
C:\wxWidgets\lib\vc_x64_lib\mswu


The exact paths depend on where wxWidgets was installed and how it was built.

Do not copy these example paths blindly; use the actual paths on the machine.

---

# Configuring wxWidgets Library Directories

Go to:


Configuration Properties
    → Linker
        → General
            → Additional Library Directories


Add the directory containing the appropriate wxWidgets `.lib` files.

For example, a wxWidgets installation might have:


C:\wxWidgets\lib\vc_x64_lib


Use the directory corresponding to the selected:

- architecture
- Debug/Release configuration
- Unicode configuration
- MSVC toolset

---

# Adding wxWidgets Libraries

Go to:


Configuration Properties
    → Linker
        → Input
            → Additional Dependencies


Add the wxWidgets libraries required by the application.

The exact library names depend on the wxWidgets version and how wxWidgets was built.

For a standard wxWidgets GUI application, the required libraries generally include the wxWidgets **core** and **base** libraries and their required dependencies.

If wxWidgets was built with Unicode and the appropriate configuration, the library names will contain the corresponding wxWidgets configuration suffixes.

> Use the library names produced by your installed wxWidgets build rather than copying library names from another wxWidgets version.

---

# Unicode Configuration

The application uses `wxString` and wxWidgets GUI controls.

Make sure the project uses the same Unicode configuration as the wxWidgets libraries.

In Visual Studio:


Project Properties
    → Configuration Properties
        → Advanced


Check:


Character Set


Use:


Use Unicode Character Set


when using the Unicode wxWidgets build.

---

# Subsystem

Because Diary is a GUI application, configure the linker as a Windows application.

Go to:


Configuration Properties
    → Linker
        → System
            → SubSystem


Select:


Windows (/SUBSYSTEM:WINDOWS)


This is important for a wxWidgets GUI application.

Do not configure the application as a console subsystem unless that is specifically intended.

---

# Windows Manifest / High DPI

If the project contains:


Diary.manifest


make sure it is included in the Visual Studio project and is passed to the linker/resource build as appropriate.

The manifest can contain Windows High-DPI settings so the application renders correctly on high-DPI displays.

This is Windows-specific and does not affect the portability of the main C++ source code.

---

# Building the Application

Select:


Build → Build Solution


or press:

Ctrl + Shift + B


A successful build should show:


Build succeeded.


---

# Running the Application

Press:


F5


to build and run with the debugger.

Or press:


Ctrl + F5


to run without debugging.

The generated executable will normally be under a configuration/platform directory such as:


x64\Debug\Diary.exe


or:


x64\Release\Diary.exe


The exact output directory depends on the Visual Studio project settings.

---

# Debugging

To debug:

1. Open the `.cpp` file where you want to inspect the program.
2. Click in the left margin to create a breakpoint.
3. Press:


F5


Visual Studio will start the application under the debugger.

Useful debugging windows include:


Debug → Windows → Locals
Debug → Windows → Watch
Debug → Windows → Call Stack


---

# Runtime Data

The application creates and manages its own runtime data files.

These may include:


settings.dat
Encryption Code.dat


and other user/backup data used by the application.

These files are runtime data and should normally **not** be included as part of the source-code submission.

If `Encryption Code.dat` does not exist, the application asks whether it should be created. Choosing **No** exits the application.

---

# Clean and Rebuild

If the project behaves strangely after changing project settings:

Select:


Build → Clean Solution


then:


Build → Rebuild Solution


This is particularly useful after changing:

- wxWidgets library paths
- Debug/Release configuration
- x86/x64 platform
- linker settings
- C++ language standard

---

## `cannot open file 'wx/...'`

The wxWidgets include directory is missing.

Check:


Project Properties
→ C/C++
→ General
→ Additional Include Directories


---

## `cannot open file 'wxmsw...'`

The wxWidgets library directory is missing or the wrong wxWidgets libraries are selected.

Check:


Project Properties
→ Linker
→ General
→ Additional Library Directories


and:


Project Properties
→ Linker
→ Input
→ Additional Dependencies


---

## Debug/Release library mismatch

Do not link Debug wxWidgets libraries into a Release configuration or vice versa.

Check the active configuration:


Debug | x64


or:


Release | x64


and make sure the corresponding wxWidgets libraries are selected.

---

## x86/x64 mismatch

Make sure the application and wxWidgets use the same architecture.

For example:


Diary: x64
wxWidgets: x64


Do not mix:


Diary: x64
wxWidgets: x86

---

# Summary

The basic Visual Studio workflow is:


1. Install Visual Studio
          ↓
2. Install Desktop development with C++
          ↓
3. Install/build wxWidgets
          ↓
4. Open Diary.sln
          ↓
5. Select x64 + Debug/Release
          ↓
6. Verify wxWidgets include/lib settings
          ↓
7. Build Solution
          ↓
8. Run with F5 or Ctrl + F5


This project does **not** require CMake when built using the Visual Studio project/solution configuration.
