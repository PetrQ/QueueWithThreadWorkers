cmake_minimum_required(VERSION 3.5)

project(QueueWithThreadWorkers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(QueueWithThreadWorkers main.cpp
    ImessageQueueEvents.h
    ThreadsManager.h
    IThreadWorker.h
    IThreadWorker.cpp
    ThreadWorker.h
    Reader.h
    Writer.h
    RingBuffer.h
    MessageQueue.h
)
