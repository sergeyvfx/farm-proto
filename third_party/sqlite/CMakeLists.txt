set(INC
	.
)

set(INC_SYS
)

set(SRC
	sqlite3.c
)

set(SRC_HEADERS
	sqlite3.h
	sqlite3ext.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(bundled_sqlite3 ${SRC} ${SRC_HEADERS})
