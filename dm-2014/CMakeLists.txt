cmake_minimum_required(VERSION 2.6)
project(exam-dm)

include_directories(
  ./
  ./common/
)

link_directories(
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/CL
)

IF (WIN32)
  SET(CL_LIBRARIES OpenCL.lib)
ELSE (WIN32)
  SET(CL_LIBRARIES OpenCL)
ENDIF(WIN32)

add_definitions(-DSRC_PATH=\"${CMAKE_SOURCE_DIR}/\")

add_executable(exam-dm exam-dm.cpp exam-dm.cl ./common/clutils.cpp)
target_link_libraries(exam-dm ${CL_LIBRARIES})
