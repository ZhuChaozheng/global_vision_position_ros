add_library(GlobalVisionLibrary SHARED 
	car.cc
	file_operation.cc
	tcp.cc
)
#target_link_libraries(GlobalVisionLibrary ${THIRD_PARTY_LIBS} #${catkin_LIBRARIES} boid_model -lpthread)

target_link_libraries(GlobalVisionLibrary ${THIRD_PARTY_LIBS} ${catkin_LIBRARIES} -lpthread)

add_executable(odometry_pub odometry_pub.cc)
add_dependencies(odometry_pub ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(odometry_pub GlobalVisionLibrary)

#add_executable(planning planning.cc)
#add_dependencies(planning ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#	target_link_libraries(planning GlobalVisionLibrary boid_model)
add_executable(tcp_driver_node tcp_driver_node.cc)
add_dependencies(tcp_driver_node ${${PROJECT_NAME}_EXPORTED_TARGETS}
	${catkin_EXPORTED_TARGETS})
target_link_libraries(tcp_driver_node GlobalVisionLibrary)

#add_executable(control_sub control_sub.cc)
#add_dependencies(control_sub ${${PROJECT_NAME}_EXPORTED_TARGETS}
#	${catkin_EXPORTED_TARGETS})
#target_link_libraries(control_sub GlobalVisionLibrary)
 
add_executable(move_client move_client.cpp)
target_link_libraries(move_client ${catkin_LIBRARIES})
add_dependencies(move_client ${PROJECT_NAME}_gencpp)
 
add_executable(move_server move_server.cpp)
target_link_libraries(move_server GlobalVisionLibrary)
add_dependencies(move_server ${PROJECT_NAME}_gencpp ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})