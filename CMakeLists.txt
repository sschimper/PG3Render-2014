cmake_minimum_required(VERSION 3.16)
project(PG3Render_2014)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(PG3Render_2014
        src/embree_util.hxx
        src/camera.hxx
        src/config.hxx
        src/directillum.hxx
        src/eyelight.hxx
        src/framebuffer.hxx
        src/geometry.hxx
        src/lights.hxx
        src/materials.hxx
        src/math.hxx
        src/pathtracer.hxx
        src/pg3render.cxx
        src/ray.hxx
        src/renderer.hxx
        src/rng.hxx
        src/scene.hxx
        src/utils.hxx src/embree_util.hxx)

option(ENABLE_EMBREE  "Use Embree for ray tracing operations?" ON)
