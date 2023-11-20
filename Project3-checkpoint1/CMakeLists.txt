cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RollerCoasters)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)

include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}glad4.6/include/)
include_directories(${INCLUDE_DIR}glm-0.9.8.5/glm/)

add_Definitions("-D_XKEYCHECK_H")
add_definitions(-DPROJECT_DIR="${PROJECT_SOURCE_DIR}")

# Executable

add_executable(RollerCoasters
    ${SRC_DIR}CallBacks.h
    ${SRC_DIR}CallBacks.cpp
    ${SRC_DIR}ControlPoint.h
    ${SRC_DIR}ControlPoint.cpp
    ${SRC_DIR}main.cpp
    ${SRC_DIR}Object.h
    ${SRC_DIR}Track.h
    ${SRC_DIR}Track.cpp
    ${SRC_DIR}TrainView.h
    ${SRC_DIR}TrainView.cpp
    ${SRC_DIR}TrainWindow.h
    ${SRC_DIR}TrainWindow.cpp
    ${INCLUDE_DIR}glad4.6/src/glad.c "src/Enum.H" "src/Draw.H" "src/Draw.cpp" "src/Enum.cpp" "src/Wave_VAO.h" "src/Wave_VAO.cpp" "src/VAO_Interface.h" "src/TextureCubeMap.h" "src/TextureCubeMap.cpp" "src/Box_VAO.h" "src/Box_VAO.cpp")


# Link Libraries

add_library(Utilities STATIC
    ${SRC_DIR}Utilities/3DUtils.h
    ${SRC_DIR}Utilities/3DUtils.cpp
    ${SRC_DIR}Utilities/ArcBallCam.h
    ${SRC_DIR}Utilities/ArcBallCam.cpp
    ${SRC_DIR}Utilities/Pnt3f.h
    ${SRC_DIR}Utilities/Pnt3f.cpp)

target_link_libraries(RollerCoasters Utilities)

target_link_libraries(RollerCoasters # Link to FLTK
    debug ${LIB_DIR}Debug/fltk_formsd.lib      optimized ${LIB_DIR}Release/fltk_forms.lib
    debug ${LIB_DIR}Debug/fltk_gld.lib         optimized ${LIB_DIR}Release/fltk_gl.lib
    debug ${LIB_DIR}Debug/fltk_imagesd.lib     optimized ${LIB_DIR}Release/fltk_images.lib
    debug ${LIB_DIR}Debug/fltk_jpegd.lib       optimized ${LIB_DIR}Release/fltk_jpeg.lib
    debug ${LIB_DIR}Debug/fltk_pngd.lib        optimized ${LIB_DIR}Release/fltk_png.lib
    debug ${LIB_DIR}Debug/fltk_zd.lib          optimized ${LIB_DIR}Release/fltk_z.lib
    debug ${LIB_DIR}Debug/fltkd.lib            optimized ${LIB_DIR}Release/fltk.lib)

target_link_libraries(RollerCoasters # Link to OpenGL
    ${LIB_DIR}OpenGL32.lib
    ${LIB_DIR}glu32.lib)

target_link_libraries(RollerCoasters # Link to OpenCV
    debug ${LIB_DIR}/Debug/opencv_world341d.lib optimized ${LIB_DIR}/Release/opencv_world341.lib
)
target_link_libraries(RollerCoasters # I don't know what it is
    ${LIB_DIR}/common.lib
    ${LIB_DIR}/ex-common.lib
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RollerCoasters)

# Git Archive
add_custom_target(
    archive
    COMMAND git archive --format=zip -osrc.zip --prefix=src/ HEAD
    COMMENT "Archive The HEAD"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
)

# Copy "assets/"
add_custom_target(CopyAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different assets ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Copying contents of assets/ to build directory..."
    VERBATIM
)
# Make CopyAssets run whenever RollerCoaster is built
add_dependencies(RollerCoasters CopyAssets)
