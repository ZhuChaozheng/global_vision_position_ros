add_library(GlobalVisionLibrary SHARED 
	car.cc
	udp.cc
	pid.cc
	timer.cc
	file_operation.cc
	navigation.cc
)
target_link_libraries(GlobalVisionLibrary ${THIRD_PARTY_LIBS} ${catkin_LIBRARIES})

#add_executable(map_pub map_pub.cc)
#add_dependencies(map_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#target_link_libraries(map_pub GlobalVisionLibrary)

add_executable(main main.cc)
add_dependencies(main ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(main GlobalVisionLibrary)

add_executable(odometry_pub odometry_pub.cc)
target_link_libraries(odometry_pub ${catkin_LIBRARIES})



#add_executable(map_pub map_pub.cc)
#add_dependencies(map_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#target_link_libraries(map_pub ${catkin_LIBRARIES})
