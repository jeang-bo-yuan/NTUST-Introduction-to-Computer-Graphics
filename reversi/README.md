# Reversi

## 使用的Library

|Name|Note|
|---|---|
|[Qt](https://www.qt.io/)|GUI視窗工具|
|[glad](https://github.com/Dav1dde/glad/tree/glad2)|動態載入OpenGL|

## Build

- Dependency

  |Name|Version|Note|
  |-----|-------|---|
  |cmake| >= 3.8 |
  |Qt| >= 5.15.2|Required Component: **Widgets**
  |C++ compiler| - | Support C++17|

- 使用Qt Creator

  以Qt Creator開啟CMakeLists.txt，選擇適當的toolkit，再點左下角的執行。

- Command Line

    ```sh
    mkdir build; cd build
    # Configure
    cmake .. -DCMAKE_BUILD_TYPE=Release
    # Build
    cmake --build .
    ```

    configure時可用的其他選項：

    |Name|Description|
    |---|---|
    |QT_MAJOR_VERSION|Qt的主版本，預設為`"5"`。若要用Qt6編譯，請設成`"6"`。|
    |CMAKE_PREFIX_PATH|若cmake找不到Qt，請將它設成安裝Qt的路徑，如：`D:\\Qt\5.15.2\msvc2019_64`。|
