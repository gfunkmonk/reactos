
add_definitions(
    -D_NTHALDLL_
    -D_NTHAL_)

include_directories(
    include
    ${REACTOS_SOURCE_DIR}/ntoskrnl/include
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/fast486)

function(add_hal _halname)
    cmake_parse_arguments(_haldata "" "" "SOURCES;COMPONENTS" ${ARGN})

    # Handle the spec file for the dll name
    spec2def(${_halname}.dll ../hal.spec ADD_IMPORTLIB)

    # Create the actual target
    if(NOT MSVC)
        foreach(_component ${_haldata_COMPONENTS})
            list(APPEND _haldata_SOURCES "$<TARGET_OBJECTS:lib_hal_${_component}>")
        endforeach()
        add_library(${_halname} MODULE
            ${_haldata_SOURCES}
            ${CMAKE_CURRENT_BINARY_DIR}/hal.def)
    else()
        foreach(_component ${_haldata_COMPONENTS})
            list(APPEND _haldata_LIBS "lib_hal_${_component}")
        endforeach()
        add_library(${_halname} MODULE
            ${_haldata_SOURCES}
            ${CMAKE_CURRENT_BINARY_DIR}/hal.def)
        target_link_libraries(${_halname} ${_haldata_LIBS})
    endif()

    if(${_halname} STREQUAL "halpc98")
        target_compile_definitions(lib_hal_pc98 PRIVATE SARCH_PC98)
    endif()

    if(${_halname} STREQUAL "halxbox")
        target_compile_definitions(lib_hal_xbox PRIVATE SARCH_XBOX)
    endif()

    if(${_halname} STREQUAL "hal")
        target_link_libraries(${_halname} libcntpr arbiter)
    else()
        target_link_libraries(${_halname} libcntpr)
    endif()

    add_importlibs(${_halname} ntoskrnl)
    #add_pch(${_halname} include/hal.h)
    add_dependencies(${_halname} psdk asm)
    set_module_type(${_halname} kerneldll ENTRYPOINT HalInitSystem 8)
    add_cd_file(TARGET ${_halname} DESTINATION reactos/system32 NO_CAB FOR all)
    if(MSVC)
        add_target_link_flags(${_halname} "/ignore:4216 /SECTION:INIT,ERWD")
    else()
        target_link_libraries(${_halname} -lgcc)
    endif()
endfunction()

# The components
include(generic.cmake)
include(acpi.cmake)
include(apic.cmake)

if(ARCH STREQUAL "i386")
    include(pcidata.cmake)
    include(legacy.cmake)
    include(hal.cmake)
    include(halapic.cmake)
    include(halmps.cmake)
    include(halacpi.cmake)
    include(halaacpi.cmake)
    include(halmacpi.cmake)
    include(xbox.cmake)
    include(pc98.cmake)
    add_subdirectory(minihal)

    remove_definitions(-DSARCH_XBOX)
    remove_definitions(-DSARCH_PC98)

    # Legacy HALs
    add_hal(hal SOURCES legacy/hal.rc COMPONENTS generic legacy hal)
    add_hal(halapic SOURCES legacy/halapic.rc COMPONENTS generic legacy halapic)
    add_hal(halmps SOURCES legacy/halmps.rc COMPONENTS generic legacy halmps)

    # ACPI HALs
    add_hal(halacpi SOURCES acpi/halacpi.rc COMPONENTS generic acpi halacpi)
    add_hal(halaacpi SOURCES acpi/halaacpi.rc COMPONENTS generic acpi halaacpi)
    add_hal(halmacpi SOURCES acpi/halmacpi.rc COMPONENTS generic acpi halmacpi)

    add_hal(halxbox SOURCES xbox/halxbox.rc COMPONENTS xbox)
    add_hal(halpc98 SOURCES pc98/halpc98.rc COMPONENTS pc98)

elseif(ARCH STREQUAL "amd64")

    list(APPEND HAL_SOURCE
        generic/spinlock.c
        amd64/x86bios.c
        amd64/halinit.c
        amd64/processor.c)

    add_hal(hal SOURCES ${HAL_SOURCE} COMPONENTS generic acpi apic)
    target_link_libraries(hal fast486)

endif()
