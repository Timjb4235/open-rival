# Development Quickstart

This is a guide to getting started with Open-Rival development.

Please also read the [contributing guidelines](/docs/CONTRIBUTING.md).

## :world_map: Project Overview

Here is a summary of all the projects in the Open-Rival repo.

### [Open-Rival](/projects/Open-Rival)

> Rival Realms remake (_the game!_).

### [Open-Rival-test](/projects/Open-Rival-test)

> Unit tests for Open-Rival.

### [setup](/projects/setup)

> Installer for Open-Rival, that combines the various utilities listed below.

### [audio-extractor](/projects/audio-extractor)

> Standalone utility to extract the original game's audio.

### [campaign-extractor](/projects/campaign-extractor)

> Standalone utility to extract individual scenarios from campaign files.

### [image-extractor](/projects/image-extractor)

> Standalone utility to extract the original game's images.

### [interface-extractor](/projects/interface-extractor)

> Standalone utility to extract the original game's interface images.

### [texture-builder](/projects/texture-builder)

> Standalone utility to combine images into textures.

## :hammer: Build

These are the steps necessary in order to build and run Open-Rival.

### 1) Install & Run CMake

The project is configured using [CMake](https://cmake.org/).

Running CMake will produce the files necessary for compilation (for example, a Visual Studio solution).

> It is recommended to use the `build` folder as the output directory for CMake, as this is excluded from the repo.

![CMake GUI](/docs/images/cmake_gui.png)

### 2) Install Dependencies

Open-Rival has various dependencies, described [here](/docs/dependencies.md).

Generally, dependencies should be placed in `projects/Open-Rival/libs` (see example below).

The expected version numbers are listed in the [CMake build file](projects/Open-Rival/CMakeLists.txt).

For dynamically-linked libraries:
- Debug DLLs should be placed in the output directory (e.g. `build/projects/Open-Rival/Debug`).
- Release DLLs should be placed in the `dist` folder.

### 3) Install Resources

At this point, it should be possible to build the solution using Visual Studio. However, the game cannot run without the required resources. Most of these are extracted from the original game by the "setup" project.

#### Setup Project

Build and run the "setup" project to perform resource extraction (instructions in the [readme](/setup/README.md)). This should create the folders `sound`, `textures` and `video` within the `setup/res` directory.

Copy these folders to the `projects/Open-Rival/res` directory.

#### Custom Resources

Some resources are custom-made for Open-Rival. These are included in the `dist/res` directory. The contents of this directory should be copied to `projects/Open-Rival/res` for use during development.

#### Font

The game uses the [Procopius font](https://fontsgeek.com/fonts/Procopius-Regular) which must be downloaded and installed separately.

### 4) That's it! :tada:

Once the dependencies and resources are in place, you should be able to run the "Open-Rival" project!

## :mag: Static Code Analysis

[Cppcheck](http://cppcheck.sourceforge.net/) should be run periodically.

> These scripts can also be found in the [tools](/tools) folder.

### Quick

```
cppcheck --project=Open-Rival.vcxproj "--project-configuration=Release|Win32"
```

### Full

```
cppcheck --project=Open-Rival.vcxproj
```

## :boom: Release

Instructions on building and packaging a version for release can be found [here](/docs/release_checklist.md).