set(name kengine_opengl)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	shaders/*.cpp shaders/*.hpp
	${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
	${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

add_library(${name} STATIC ${src})
target_compile_definitions(${name} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

putils_conan(
	freetype/2.10.0@bincrafters/stable
	glfw/3.3@bincrafters/stable
)
target_link_libraries(${name} kengine CONAN_PKG::glfw CONAN_PKG::freetype)