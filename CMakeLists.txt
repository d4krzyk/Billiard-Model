cmake_minimum_required(VERSION 3.8...3.12 FATAL_ERROR)
project(MinimalQtVTKApp)

find_package(VTK
  COMPONENTS
    CommonCore
    CommonTransforms
    CommonColor
    GUISupportQt
    IOImage
    ImagingCore
    FiltersTexture
    FiltersSources
    InteractionStyle
    InteractionWidgets
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
)
if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()

find_package("Qt${VTK_QT_VERSION}" COMPONENTS Core Widgets)
if (NOT TARGET "Qt${VTK_QT_VERSION}::Core" OR NOT TARGET "Qt${VTK_QT_VERSION}::Widgets")
  message("Skipping example: ${Qt${VTK_QT_VERSION}_NOT_FOUND_MESSAGE}")
  return ()
endif ()

# Set your files and resources here
set(Srcs
  MinimalQtVTKApp.cxx)

# Określamy, gdzie znajdują się tekstury (cały folder)
set(TEXTURE_DIR ${CMAKE_SOURCE_DIR}/textures)

# Kopiowanie plików z folderu 'textures' do folderu build
file(GLOB TEXTURE_FILES "${TEXTURE_DIR}/*.jpg")
file(COPY ${TEXTURE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/textures)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


# CMAKE_AUTOMOC in ON so the MocHdrs will be automatically wrapped.
add_executable(MinimalQtVTKApp
  ${Srcs})
target_link_libraries(MinimalQtVTKApp
  PRIVATE
    ${VTK_LIBRARIES}
    "Qt${VTK_QT_VERSION}::Core"
    "Qt${VTK_QT_VERSION}::Widgets")
vtk_module_autoinit(
  TARGETS MinimalQtVTKApp
  MODULES ${VTK_LIBRARIES})
