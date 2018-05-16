PROJECT_TARGET_ADD(rover-menu)

# Define project Targets
ADD_EXECUTABLE(${TARGET_NAME} src/main.cpp src/appstacle_logo.c)
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
	LABELS "EXECUTABLE"
	OUTPUT_NAME ${TARGET_NAME}
)

# Library dependencies (include updates automatically)
TARGET_LINK_LIBRARIES(${TARGET_NAME}
	${link_libraries} rover-services-api ${libafbwsc_LIBRARIES}
)
