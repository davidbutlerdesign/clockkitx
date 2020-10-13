cmake_minimum_required(VERSION 3.15)

project(ClockKitX
	LANGUAGES CXX)

# Install dependenciess
if(NOT EXISTS ${CMAKE_BINARY_DIR}/conan.cmake)
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
	PRIVATE
		"src/Clock.cpp"
		"src/ClockClient.cpp"
		"src/ClockClientMain.cpp"
		
		"src/ClockPacket.cpp"
		"src/ClockServer.cpp"
		"src/ConfigReader.cpp"
		"src/Exceptions.cpp"
		"src/PerformanceCounter.cpp"
		"src/PhaseLockedClock.cpp"
		"src/SystemClock.cpp"
		"src/Timestamp.cpp"
		"src/VariableFrequencyClock.cpp"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		CONAN_PKG::poco
)

set_target_properties(${PROJECT_NAME} 
	PROPERTIES 
		DEBUG_POSTFIX "d"
)

target_compile_features(${PROJECT_NAME}
	PUBLIC
		cxx_std_14
)

add_subdirectory(examples)