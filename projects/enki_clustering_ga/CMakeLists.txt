if (SDL2_FOUND)
	message(STATUS "SDL2 found, enabling joystick support")
	include_directories(${SDL2_INCLUDE_DIR})
	set(EXTRA_LIBS ${SDL2_LIBRARY} ${EXTRA_LIBS})
	add_definitions("-DUSE_SDL")
endif ()

if (Qt5Widgets_FOUND AND Qt5OpenGL_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(viewer_playground_SRCS main.cpp)

	include_directories(${PROJECT_SOURCE_DIR})

	add_executable(enki_clustering_ga ${viewer_playground_SRCS})

	target_link_libraries(enki_clustering_ga enkiviewer enki ${OPENGL_gl_LIBRARY} ${EXTRA_LIBS} Qt5::OpenGL Qt5::Widgets)
endif ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/run.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
