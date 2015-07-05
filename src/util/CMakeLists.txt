set(INC
	.
)

set(INC_SYS
)

set(SRC
	util_json.cc
	util_logging.cc
	util_path.cc
	util_string.cc
	util_time.cc
	util_uri.cc
)

set(SRC_HEADERS
	util_algorithm.h
	util_foreach.h
	util_json.h
	util_logging.h
	util_map.h
	util_path.h
	util_priority_queue.h
	util_string.h
	util_thread.h
	util_time.h
	util_uri.h
	util_vector.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(farm_util ${SRC} ${SRC_HEADERS})
