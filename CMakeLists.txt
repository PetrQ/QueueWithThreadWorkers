cmake_minimum_required(VERSION 3.5)

project(TestCallBackManage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestCallBackManage main.cpp ImessageQueueEvents.h ThreadWorker.h IThreadWorker.h RingBuffer2.h)
