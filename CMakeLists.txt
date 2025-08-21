cmake_minimum_required(VERSION 3.16)
project(WorldsOfWhite LANGUAGES C CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(Bullet REQUIRED) # Prefer config package, fallback to Find module


# GLFW - prefer CMake config, fallback to pkg-config
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLFW3 REQUIRED IMPORTED_TARGET glfw3)
endif()

# Sources (mirrors the Visual Studio project)
set(EXPLORATION_SOURCES
  exploration/main.cpp
  exploration/InputManager.cpp
  exploration/utilities/ring.cpp
  exploration/libraries/glad/src/glad.c
  exploration/logging/LoggingManager.cpp
  exploration/logging/SourceLogger.cpp
  exploration/logging/loggers/FileLogger.cpp
  exploration/logging/loggers/StreamLogger.cpp
  exploration/graphics/shader.cpp
  exploration/graphics/program.cpp
  exploration/graphics/texture.cpp
  exploration/graphics/framebuffer.cpp
  exploration/graphics/joint.cpp
  exploration/graphics/jointPose.cpp
  exploration/graphics/programs/ScreenProgram.cpp
  exploration/graphics/programs/DepthProgram.cpp
  exploration/graphics/programs/DebugProgram.cpp
  exploration/graphics/programs/LineProgram.cpp
  exploration/Model.cpp
  exploration/entities/AnimatedEntity.cpp
  exploration/entities/PlayerEntity.cpp
  exploration/entities/DecorationEntity.cpp
  exploration/entities/Entity.cpp
  exploration/entities/PhysicsEntity.cpp
  exploration/entities/SpiritEntity.cpp
  exploration/entities/SmashEffectEntity.cpp
  exploration/entities/TerrainEntity.cpp
  exploration/cameras/FollowCamera.cpp
  exploration/cameras/FreeCamera.cpp
  exploration/cameras/TrackCamera.cpp
  exploration/cameras/IdleCamera.cpp
)

add_executable(exploration ${EXPLORATION_SOURCES})

# Include directories: project headers + vendored header-only libs
# (GLFW include dir comes from the package, not the vendored headers in this repo)
target_include_directories(exploration PRIVATE
  build

  exploration
  exploration/libraries/glad/include
  exploration/libraries/glm/include
  exploration/libraries/cimg/include
  
  ${BULLET_INCLUDE_DIRS}
)

# CImg setup: don't pull X11 display backends; enable JPEG loader
# This avoids linking to X11 while still allowing texture loading from JPEG.
target_compile_definitions(exploration PRIVATE cimg_display=0 cimg_use_jpeg)

# Link libraries
if(glfw3_FOUND)
  # Different distros export different target names; support common ones
  if(TARGET glfw)
    target_link_libraries(exploration PRIVATE glfw)
  elseif(TARGET glfw3::glfw)
    target_link_libraries(exploration PRIVATE glfw3::glfw)
  elseif(TARGET GLFW::GLFW)
    target_link_libraries(exploration PRIVATE GLFW::GLFW)
  else()
    message(FATAL_ERROR "glfw3 was found but no known CMake target is available.")
  endif()
else()
  target_link_libraries(exploration PRIVATE PkgConfig::GLFW3)
endif()

# OpenGL, JPEG, Bullet
# Prefer Bullet imported target if available, else fall back to variables
if(TARGET Bullet::Bullet)
  target_link_libraries(exploration PRIVATE OpenGL::GL JPEG::JPEG Bullet::Bullet)
elseif(DEFINED BULLET_LIBRARIES)
  target_link_libraries(exploration PRIVATE OpenGL::GL JPEG::JPEG ${BULLET_LIBRARIES})
else()
  # Last resort: try common Bullet components
  target_link_libraries(exploration PRIVATE OpenGL::GL JPEG::JPEG BulletCollision BulletDynamics LinearMath)
endif()

# On some systems Bullet does not provide imported targets; ensure PIC where needed
set_property(TARGET exploration PROPERTY POSITION_INDEPENDENT_CODE ON)

# Convenience run target to run from build/ with asset symlinks
add_custom_target(run
  COMMAND ${CMAKE_COMMAND} -E env zsh ${CMAKE_SOURCE_DIR}/scripts/run_from_build.zsh ${CMAKE_BINARY_DIR}
  DEPENDS exploration
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  USES_TERMINAL
)

# Install rule (optional)
install(TARGETS exploration RUNTIME DESTINATION bin)
