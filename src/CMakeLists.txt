cmake_minimum_required(VERSION 2.8)

project(RayTracing)

add_executable(RayTracer.o RayTracer.cpp Cone.cpp TextureBMP.cpp Cylinder.cpp Plane.cpp Ray.cpp SceneObject.cpp Sphere.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( RayTracer.o ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
