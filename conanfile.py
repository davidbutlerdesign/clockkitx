from conans import ConanFile, CMake

class ClockKitXConan(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	generators = "cmake_multi", "cmake"
	
	requires = (
		"poco/1.10.1"
		)
		
	default_options = (
		)
	