set(INC
	.
)

set(INC_SYS
	${GLIB2_INCLUDE_DIRS}
	${LIBSOUP_INCLUDE_DIRS}
)

set(SRC
	http_server_soup.cc
)

set(SRC_HEADERS
	http_server.h
	http_server_soup.h
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

add_library(farm_http ${SRC} ${SRC_HEADERS})
