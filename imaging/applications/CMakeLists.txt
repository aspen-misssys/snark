set( dir ${SOURCE_CODE_BASE_DIR}/imaging/applications )
file( GLOB source   ${dir}/*.cpp )
file( GLOB includes ${dir}/*.h )
file( GLOB cv_calc_source ${dir}/cv_calc/*.cpp )
file( GLOB cv_calc_include ${dir}/cv_calc/*.h )

if( snark_build_imaging_stereo ) # todo: add proper cmake to stereo
    file( GLOB stereo_source ${dir}/stereo/*.cpp )
    file( GLOB stereo_includes ${dir}/stereo/*.h )
    add_executable( stereo-to-points stereo-to-points.cpp ${stereo_source}  )
    target_link_libraries( stereo-to-points snark_imaging snark_math ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} )
    install( TARGETS stereo-to-points RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )
endif( snark_build_imaging_stereo )

SOURCE_GROUP( ${TARGET_NAME} FILES ${source} ${includes} ${cv_calc_source} ${cv_calc_include} )

ADD_EXECUTABLE( cv-cat cv-cat.cpp )
TARGET_LINK_LIBRARIES( cv-cat snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} tbb )

ADD_EXECUTABLE( cv-calc cv-calc.cpp ${cv_calc_source} ${cv_calc_includes} )
TARGET_LINK_LIBRARIES( cv-calc snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} )

ADD_EXECUTABLE( image-edit image-edit.cpp )
TARGET_LINK_LIBRARIES( image-edit snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} )
install( TARGETS image-edit RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )

ADD_EXECUTABLE( image-color-calc image-color-calc.cpp )
TARGET_LINK_LIBRARIES( image-color-calc snark_imaging ${comma_ALL_LIBRARIES} ${Boost_LIBRARIES} )

add_executable( image-from-csv image-from-csv.cpp )
target_link_libraries( image-from-csv snark_imaging ${comma_ALL_LIBRARIES} )
install( TARGETS image-from-csv RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )

ADD_EXECUTABLE( image-to-csv image-to-csv.cpp )
TARGET_LINK_LIBRARIES( image-to-csv snark_imaging ${comma_ALL_LIBRARIES} )

ADD_EXECUTABLE( image-accumulate image-accumulate.cpp )
TARGET_LINK_LIBRARIES( image-accumulate snark_imaging ${comma_ALL_LIBRARIES} tbb )

INSTALL( TARGETS cv-cat cv-calc image-accumulate image-color-calc image-to-csv
         RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR}
         COMPONENT Runtime )
         
add_executable( image-pinhole image-pinhole.cpp )
target_link_libraries( image-pinhole snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ) #target_link_libraries( image-pinhole snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} tbb )
install( TARGETS image-pinhole RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )

add_executable( image-pinhole-calibrate image-pinhole-calibrate.cpp )
target_link_libraries( image-pinhole-calibrate snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} )
install( TARGETS image-pinhole-calibrate RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )

add_executable( image-stereo image-stereo.cpp )
target_link_libraries( image-stereo snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} ) #target_link_libraries( image-pinhole snark_imaging ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} tbb )
install( TARGETS image-stereo RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )

#add_executable( equirectangular-to-rectilinear equirectangular-to-rectilinear.cpp )
#target_link_libraries( equirectangular-to-rectilinear ${comma_ALL_LIBRARIES} ${OpenCV_LIBS} )
#install( TARGETS equirectangular-to-rectilinear RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )
