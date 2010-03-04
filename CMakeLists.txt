#Define the project we are working on
project(glapp_framework)

# Look for various software we need 
cmake_minimum_required(VERSION 2.6)

find_package(PythonInterp)

if(NOT PYTHONINTERP_FOUND OR NOT PYTHON_EXECUTABLE)
  message(FATAL_ERROR "No python interpreter found .. needed for generating shader include files")
endif(NOT PYTHONINTERP_FOUND OR NOT PYTHON_EXECUTABLE)

find_package(PythonLibs)

if(NOT PYTHONLIBS_FOUND)
  message(FATAL_ERROR "Python libs not found")
endif(NOT PYTHONLIBS_FOUND)

find_package(Boost COMPONENTS python )

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Cannot find boost")
endif(NOT Boost_FOUND)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 REQUIRED)

if(NOT QT4_FOUND)
  message(FATAL_ERROR "Failed to find Qt 4.5 or greater.")
endif(NOT QT4_FOUND)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTOPENGL TRUE)

include(${QT_USE_FILE})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(GLEW)

if(NOT GLEW_FOUND)
  message(FATAL_ERROR "Failed to find GLEW .. quitting")
endif(NOT GLEW_FOUND)

find_package(Freetype)

if(FREETYPE_FOUND)
  find_package(FTGL)
endif(FREETYPE_FOUND)

find_package(GLUT)
  
if(NOT FTGL_FOUND AND NOT GLUT_FOUND)
  message(FATAL_ERROR "Failed to find glut or FTGL libraries.. quitting")
endif(NOT FTGL_FOUND AND NOT GLUT_FOUND)


# generate the headerfile containing the shader sources

file(GLOB shader_FILES ${CMAKE_CURRENT_SOURCE_DIR}/utls/shaders/*)

set(shader_SRCS ${CMAKE_CURRENT_BINARY_DIR}/shadersources.h ${CMAKE_CURRENT_BINARY_DIR}/shadersources.cpp)
		   
add_custom_command(
  OUTPUT ${shader_SRCS}
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/utls/copy_files_to_c_header.py 
  ${CMAKE_CURRENT_BINARY_DIR}/shadersources ${shader_FILES}		   
  DEPENDS ${shader_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/utls/copy_files_to_c_header.py)

include_directories(${CMAKE_CURRENT_BINARY_DIR}) 

#generate python bindings

file(GLOB python_binding_FILES ${CMAKE_CURRENT_SOURCE_DIR}/framework/include/*.h)

set(framework_python_binding_SRCS ${CMAKE_CURRENT_BINARY_DIR}/python_bindings.cpp)

add_custom_command(
  OUTPUT ${framework_python_binding_SRCS}
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/framework/generate_code.py 
  "-o" ${framework_python_binding_SRCS}
  "-m" ${PROJECT_NAME}
  "-I" ${CMAKE_CURRENT_SOURCE_DIR}/framework/include/
  "-D" "GLAFW_GENERATING_PYTHON_BINDINGS"
  ${python_binding_FILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${python_binding_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/framework/generate_code.py)

# add in all other sources

file(GLOB utls_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/utls/src/*.cpp 
                    ${CMAKE_CURRENT_SOURCE_DIR}/utls/src/*.h 
		    ${CMAKE_CURRENT_SOURCE_DIR}/utls/include/*.h)

set(utls_SRCS ${utls_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/shadersources.h)

file(GLOB tristripper_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tristripper/*.cpp
			   ${CMAKE_CURRENT_SOURCE_DIR}/tristripper/*.h 
			   ${CMAKE_CURRENT_SOURCE_DIR}/tristripper/detail/*.h)

file(GLOB qtcolorpicker_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/src/*.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/src/*.h 
		             ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/include/*.h)

QT4_WRAP_CPP(qtcolorpicker_MOC_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/include/qtcolorpicker.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/src/qtcolorpicker_widgets.h  
  )

file(GLOB framework_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/*.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/*.h 
		         ${CMAKE_CURRENT_SOURCE_DIR}/framework/include/*.h)


QT4_WRAP_CPP(  framework_MOC_SRCS 
   ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/framework_impl.h
   ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/Mainwindow.h
   ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/glwidget.h
   ${CMAKE_CURRENT_SOURCE_DIR}/framework/src/light_impl.h
   )

file(GLOB framework_UIS ${CMAKE_CURRENT_SOURCE_DIR}/framework/forms/*.ui)

QT4_WRAP_UI(framework_UI_SRCS ${framework_UIS})

QT4_ADD_RESOURCES(framework_RC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/framework/resources/framework.qrc)

include_directories(
  ${QT_ADDITIONAL_INCLUDE_PATHS}
  ${GLEW_INCLUDE_PATH}
  ${FREETYPE_INCLUDE_DIRS}
  ${FTGL_INCLUDE_PATH}
  ${GLUT_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/utls/include
  ${CMAKE_CURRENT_SOURCE_DIR}/utls/src 
  ${CMAKE_CURRENT_SOURCE_DIR}/tristripper
  ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/include
  ${CMAKE_CURRENT_SOURCE_DIR}/qtcolorpicker/src
  ${CMAKE_CURRENT_SOURCE_DIR}/framework/include
  ${CMAKE_CURRENT_SOURCE_DIR}/framework/src  
  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_library(${PROJECT_NAME} SHARED 
  ${shader_SRCS}
  ${utls_SRCS} 
  ${tristripper_SRCS} 
  ${qtcolorpicker_SRCS} 
  ${qtcolorpicker_MOC_SRCS}
  ${framework_SRCS} 
  ${framework_MOC_SRCS}
  ${framework_UI_SRCS} 
  ${framework_RC_SRCS}  
  ${framework_python_binding_SRCS}
  )

target_link_libraries(${PROJECT_NAME}
  ${QT_LIBRARIES} 
  ${QT_ADDITIONAL_LIBRARIES} 
  ${GLEW_LIBRARY}
  ${GLUT_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${FTGL_LIBRARY} 
  ${PYTHON_LIBRARIES}
  ${Boost_LIBRARIES}    
  )

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")