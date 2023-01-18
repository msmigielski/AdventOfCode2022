project(UT_Day12)

set(DATA_FILE day12_data.txt)

add_executable(${PROJECT_NAME}
  task.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/${DATA_FILE}
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${DATA_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${DATA_FILE}
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${DATA_FILE}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Catch2::Catch2WithMain
)

add_test(${PROJECT_NAME} ${PROJECT_NAME})