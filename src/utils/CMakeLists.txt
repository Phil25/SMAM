project(SMAM_src_utils)

function(create_utils_library libName)
  add_library(utils_${libName})
  target_sources(utils_${libName}
    PRIVATE ${libName}.cpp
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${libName}.h
  )
  target_include_directories(utils_${libName} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
endfunction()

create_utils_library(archive)
create_utils_library(misc)
create_utils_library(printer)
create_utils_library(smfs)
create_utils_library(version)
