# simple CMake file for quicksort experiments

set(SOURCES qstest.cc)

# if have PAPI, set includes, library and -D define switch
if(PAPI_FOUND)
  include_directories(${PAPI_INCLUDE_DIRS})
  add_definitions("-DWITH_PAPI")
  set(LIBS ${LIBS} ${PAPI_LIBRARIES})
endif()

if(WITH_MALLOC_COUNT)

  add_definitions(-DMALLOC_COUNT)
  list(APPEND SOURCES
    tools/malloc_count.c
    tools/stack_count.c
  )
  list(APPEND LIBS "dl")

endif(WITH_MALLOC_COUNT)

add_executable(qstest ${SOURCES})
target_link_libraries(qstest ${LIBS})

