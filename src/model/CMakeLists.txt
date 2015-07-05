set(INC
	.
)

set(INC_SYS
	${GLIB2_INCLUDE_DIRS}
	${LIBSOUP_INCLUDE_DIRS}
)

set(SRC
	model_farm.cc
	model_job.cc
	model_task.cc
)

set(SRC_HEADERS
	model_farm.h
	model_job.h
	model_task.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(farm_model ${SRC} ${SRC_HEADERS})
