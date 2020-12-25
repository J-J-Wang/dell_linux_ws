cmake_minimum_required(VERSION 2.8.3)
project(redwall_robot_e_control)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")


#find_package(GTSAM REQUIRED QUIET)
#find_package(PCL REQUIRED QUIET)
#find_package(OpenCV REQUIRED QUIET)




include_directories(
	#${PCL_INCLUDE_DIRS}
	#${OpenCV_INCLUDE_DIRS}
	#${GTSAM_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/include
)


link_directories(
	#${OpenCV_LIBRARY_DIRS}
	#${PCL_LIBRARY_DIRS}
	#${GTSAM_LIBRARY_DIRS}
)



find_package(Threads REQUIRED)


add_executable(redwall_robot_e_control main.cpp snap7.cpp)
target_link_libraries(redwall_robot_e_control  ${PROJECT_SOURCE_DIR}/lib/libsnap7.so  Threads::Threads)

#add_executable(extrating_indices extrating_indices.cpp)
#target_link_libraries(extrating_indices ${PCL_LIBRARIES} )
