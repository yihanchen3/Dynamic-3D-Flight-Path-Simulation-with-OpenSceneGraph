project("osg_cruise_simulation")
 
set(OSG_DIR "${CMAKE_SOURCE_DIR}/osg")
include_directories(${OSG_DIR}/include)
link_directories(${OSG_DIR}/lib)
 
add_executable(${PROJECT_NAME} flight.cpp "flight.cpp")
 
target_link_libraries(${PROJECT_NAME} 
	osgViewerd
	osgd
	osgDBd
	osgGAd
	osgUtild
)
 
file(GLOB OSG_PLUGIN_DLLS ${OSG_DIR}/bin/osgPlugins-*/osgdb_*.dll)
file(COPY ${OSG_PLUGIN_DLLS} DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB OSG_DLLS ${OSG_DIR}/bin/*.dll)
file(COPY ${OSG_DLLS} DESTINATION ${CMAKE_BINARY_DIR})