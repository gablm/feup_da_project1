cmake_minimum_required(VERSION 3.22.1)
project(first_project_DA)

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
endif (UNIX)

add_executable(first_project_DA
        src/ui.cpp
        src/managerLoad.cpp
        src/managerWork.cpp
        src/managerReliability.cpp
        src/managerPipes.cpp
		src/uiMaxFlow.cpp
		src/uiMeetDemands.cpp
		src/uiBalanceNetwork.cpp
		src/uiRemoveReservoir.cpp
		src/uiRemovePS.cpp
		src/uiRemovePipe.cpp
)
