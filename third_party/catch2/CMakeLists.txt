# Download content during cmake configuration
Include(FetchContent)

set(FETCHCONTENT_QUIET off)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/third_party/catch2)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.1.1
)

FetchContent_MakeAvailable(Catch2)