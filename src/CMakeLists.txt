add_library(GlobalVisionLibrary SHARED 
	car.cc
	udp.cc
	pid.cc
	timer.cc
	file_operation.cc
	navigation.cc
)
target_link_libraries(GlobalVisionLibrary ${THIRD_PARTY_LIBS} ${catkin_LIBRARIES} boid_model)

#add_executable(map_pub map_pub.cc)
#add_dependencies(map_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#target_link_libraries(map_pub GlobalVisionLibrary)

add_executable(main main.cc)
add_dependencies(main ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(main GlobalVisionLibrary)

add_executable(odometry_pub odometry_pub.cc)
add_dependencies(odometry_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(odometry_pub GlobalVisionLibrary)

add_executable(planning planning.cc)
add_dependencies(planning ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(planning GlobalVisionLibrary boid_model)

add_executable(control_sub control_sub.cc)
add_dependencies(control_sub ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(control_sub GlobalVisionLibrary)

#add_executable(map_pub map_pub.cc)
#add_dependencies(map_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#target_link_libraries(map_pub ${catkin_LIBRARIES})
