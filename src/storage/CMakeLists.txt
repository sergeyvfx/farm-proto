set(INC
	.
)

set(INC_SYS
	../../third_party
)

set(SRC
	storage_database_sqlite.cc
	storage_dryrun.cc
)

set(SRC_HEADERS
	storage.h
	storage_database.h
	storage_database_sqlite.h
	storage_dryrun.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(farm_storage ${SRC} ${SRC_HEADERS})
