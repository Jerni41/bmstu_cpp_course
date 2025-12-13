message(STATUS "Running tasks/bmstu_list/CMakeLists.txt")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(NAME_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Добавьте этот блок для поиска includes по всему проекту
set(INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/includes
    ${PROJECT_SOURCE_DIR}/tasks
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#save all folders in tasks with prefix task_ to array
file(GLOB TASKS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/task_*)

foreach (TASK ${TASKS})
    message(STATUS "FIND IN: " ${TASK})
    file(GLOB FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}/*.[ch]pp
            ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}/*.h
            ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}/*.natvis)
    list(APPEND SOURCES ${FILES})
    # Добавляем путь к папке задачи в include пути
    list(APPEND INCLUDE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/${TASK})
endforeach ()

message(STATUS "SOURCES: ${SOURCES}")

# Просмотрим все найденные include пути
message(STATUS "INCLUDE PATHS:")
foreach(PATH ${INCLUDE_PATHS})
    message(STATUS "  - ${PATH}")
endforeach()

add_executable(${NAME_EXECUTABLE} ${SOURCES})

# Добавляем ВСЕ найденные пути к include директориям
target_include_directories(${NAME_EXECUTABLE} PUBLIC ${INCLUDE_PATHS})

# Если есть другие зависимости, добавьте их здесь
target_include_directories(${NAME_EXECUTABLE} PUBLIC ${PROJECT_SOURCE_DIR}/tasks/bmstu_abstract_iterator/task_abstract_iterator)

# Добавьте путь к optional если он есть в другом месте
if(EXISTS ${PROJECT_SOURCE_DIR}/tasks/bmstu_optional/task_optional)
    target_include_directories(${NAME_EXECUTABLE} PUBLIC ${PROJECT_SOURCE_DIR}/tasks/bmstu_optional/task_optional)
endif()

target_link_libraries(
        ${NAME_EXECUTABLE}
        GTest::gtest_main
)