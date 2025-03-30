
add_library(SpaceBattle STATIC
  "ICommand.h"
  "CommandException.cpp" "CommandException.h"
  "ExceptionHandler.cpp" "ExceptionHandler.h"
  )

add_subdirectory (IoCContainer)
add_subdirectory (CmdExecutor)

target_link_libraries(SpaceBattle PRIVATE
	IoC
	CmdExecutor
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SpaceBattle PROPERTY CXX_STANDARD 20)
endif()

