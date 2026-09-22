# cmake/KindlePackaging.cmake
# Kindle OTA packaging integration for Kindle Launchpad

find_program(KINDLETOOL_EXECUTABLE kindletool
    DOC "Path to the kindletool binary for Kindle OTA package creation"
)

set(KINDLE_TARGET_MODELS k2 k2i dx dxi dxg k3g k3w k3gb CACHE STRING "Target Kindle device models for OTA updates")
set(KINDLE_PACKAGE_VERSION "0.0.2" CACHE STRING "Kindle Launchpad package version")

set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging")
set(STAGING_LP_DIR "${STAGING_DIR}/src/launchpad")
set(PACKAGES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/packages")

add_custom_target(package-stage
    COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_LP_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:launchpad>" "${STAGING_LP_DIR}/launchpad"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/launchpad.ini" "${STAGING_LP_DIR}/launchpad.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/servicecmds.ini" "${STAGING_LP_DIR}/servicecmds.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/rce.ini" "${STAGING_LP_DIR}/rce.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/fbreader.ini" "${STAGING_LP_DIR}/fbreader.ini"
    COMMENT "Staging Launchpad binaries and default configuration files"
    DEPENDS launchpad
)

if(KINDLETOOL_EXECUTABLE)
    message(STATUS "Found kindletool: ${KINDLETOOL_EXECUTABLE} - OTA packaging enabled")

    add_custom_target(package-ota
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGES_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E tar "czf" "${CMAKE_BINARY_DIR}/launchpad.tar.gz" -C "${STAGING_DIR}" "src"
        COMMENT "Generating Kindle OTA update packages"
        DEPENDS package-stage
    )

    foreach(MODEL ${KINDLE_TARGET_MODELS})
        add_custom_command(TARGET package-ota POST_BUILD
            COMMAND ${KINDLETOOL_EXECUTABLE} create ota -d ${MODEL} -s
                -b "${CMAKE_CURRENT_SOURCE_DIR}/packaging/src/launchpad-init"
                "${CMAKE_BINARY_DIR}/launchpad.tar.gz"
                "${CMAKE_CURRENT_SOURCE_DIR}/packaging/src/install.sh"
                "${PACKAGES_OUTPUT_DIR}/update_launchpad_${KINDLE_PACKAGE_VERSION}_${MODEL}_install.bin"
            COMMAND ${KINDLETOOL_EXECUTABLE} create ota -d ${MODEL} -s
                "${CMAKE_CURRENT_SOURCE_DIR}/packaging/src/uninstall.sh"
                "${PACKAGES_OUTPUT_DIR}/update_launchpad_${KINDLE_PACKAGE_VERSION}_${MODEL}_uninstall.bin"
            COMMENT "Building signed OTA packages for model ${MODEL}"
        )
    endforeach()
else()
    add_custom_target(package-ota
        COMMAND ${CMAKE_COMMAND} -E echo "kindletool not found in PATH."
        COMMAND ${CMAKE_COMMAND} -E echo "Staging is available via package-stage target."
        COMMAND ${CMAKE_COMMAND} -E echo "To generate signed .bin packages, install kindletool from NiLuJe/KindleTool."
        COMMENT "Kindle OTA packaging requires kindletool"
        DEPENDS package-stage
    )
endif()
