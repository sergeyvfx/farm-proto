set(INC
	.
)

set(INC_SYS
	${GLIB2_INCLUDE_DIRS}
	${LIBSOUP_INCLUDE_DIRS}
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_executable(farm_server farm_server.cc)
target_link_libraries(farm_server
                      farm_storage
                      farm_model
                      farm_http
                      farm_util
                      bundled_sqlite3
                      ${GLIB2_LIBRARIES}
                      ${LIBSOUP_LIBRARIES}
                      ${GLOG_LIBRARIES}
                      ${GFLAGS_LIBRARIES}
                      ${PTHREADS_LIBRARIES}
                      ${CMAKE_DL_LIBS})

add_executable(farm_worker_dummy farm_worker_dummy.cc)
target_link_libraries(farm_worker_dummy
                      farm_storage
                      farm_model
                      farm_http
                      farm_util
                      bundled_sqlite3
                      ${GLIB2_LIBRARIES}
                      ${LIBSOUP_LIBRARIES}
                      ${GLOG_LIBRARIES}
                      ${GFLAGS_LIBRARIES}
                      ${PTHREADS_LIBRARIES}
                      ${CMAKE_DL_LIBS})
