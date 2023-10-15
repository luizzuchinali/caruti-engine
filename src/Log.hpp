#pragma once

#include "fmt/format.h"
#include "fmt/color.h"

class Log {
public:
    template<typename T>
    static void Error(const T &data) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "Error: ");
        fmt::print("{}\n", data);
    }
    template<typename T, typename T2>
    static void Error(const T &temp, T2 &data) {
        Error(fmt::format(fmt::runtime(temp), data));
    }

    template<typename T>
    static void Information(const T &data) {
        fmt::print("{}\n", data);
    }

    template<typename T, typename T2>
    static void Information(const T &temp, const T2 &data) {
        Information(fmt::format(temp, data));
    }

    template<typename T, typename T2>
    static void Information(const T &temp, T2 &data) {
        Information(fmt::format(fmt::runtime(temp), data));
    }
};