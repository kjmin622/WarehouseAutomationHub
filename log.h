#pragma once
#include<iostream>

#define DEBUG

#ifdef DEBUG
#define LOG(message) std::cout << "[LOG] " << message << std::endl
#define ERROR(message) std::cout << "\033[31m[ERROR]\033[0m " << message << std::endl
#define WARNING(message) std::cout << "\033[33m[WARNING]\033[0m " << message << std::endl
#else
#define LOG(message)
#define ERROR(message)
#define WARNING(message)
#endif
