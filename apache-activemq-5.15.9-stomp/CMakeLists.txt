cmake_minimum_required(VERSION 2.8.9)
project (stompEx)

include_directories(
  ../activemq-cpp-library-3.9.5
  ../activemq-cpp-library-3.9.5/src/main/
  ../apr-1.6.5
  ../apr-1.6.5/include
)

set ( PROJECT_LINK_LIBS
  libactivemq-cpp.lib
  libapr-1.lib
  Ws2_32.lib
)

link_directories( 
../activemq-cpp-library-3.9.5/vs2010-build/x64/Release
../apr-1.6.5/Release
)

add_executable(stompExListener Listener.cpp)
target_link_libraries(stompExListener ${PROJECT_LINK_LIBS} )

add_executable(stompExPublisher Publisher.cpp)
target_link_libraries(stompExPublisher ${PROJECT_LINK_LIBS} )


