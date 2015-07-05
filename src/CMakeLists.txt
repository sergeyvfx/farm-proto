# External libraries

include(cmake/external_libs.cmake)

include_directories(.)

# Logging capabilities using GLog library.
add_definitions(-DGOOGLE_GLOG_DLL_DECL=)
add_definitions(-DFARM_GFLAGS_NAMESPACE=${GFLAGS_NAMESPACE})
include_directories(
  SYSTEM
  ${GLOG_INCLUDE_DIRS}
  ${GFLAGS_INCLUDE_DIRS}
)

add_subdirectory(app)
add_subdirectory(http)
add_subdirectory(model)
add_subdirectory(storage)
add_subdirectory(util)
