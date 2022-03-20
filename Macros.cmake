function(tempeh_add_shader_module _TARGET_NAME _STAGE _SRC)
    if(_STAGE STREQUAL "VERTEX")
        set(_STAGE "vert")
    elseif(_STAGE STREQUAL "FRAGMENT")
        set(_STAGE "frag")
    else()
        message(FATAL_ERROR "Unknown shader stage")
    endif()
    get_filename_component(_SRC_FILE "${_SRC}" ABSOLUTE)
    # message("${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -S ${_STAGE} --target-env spirv1.0 -o ${CMAKE_BINARY_DIR} ${_SRC_FILE}")
    add_custom_target(${_TARGET_NAME}
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V -S ${_STAGE} --target-env spirv1.0 -o "${CMAKE_CURRENT_BINARY_DIR}/${_TARGET_NAME}.spv" "${_SRC_FILE}"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/${_TARGET_NAME}.spv"
        DEPENDS "${_SRC_FILE}"
        SOURCES "${_SRC_FILE}")
endfunction()

function(tempeh_add_dependency_group)
    if(ARGC EQUAL 0)
        message(FATAL_ERROR "No name given for dependency group")
    endif()

    if(ARGC LESS 2)
        message(FATAL_ERROR "Dependency group must at least contain 1 dependency")
    endif()

    set(_GROUP_NAME ${ARGV0})
    add_custom_target(${_GROUP_NAME})
    list(POP_FRONT ARGN) # pop the first argument

    foreach(_I IN LISTS ARGN)
        add_dependencies(${_GROUP_NAME} ${_I})
    endforeach()
endfunction()
