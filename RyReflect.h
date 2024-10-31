/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 生成代码文件的程序
 * @github https://github.com/ZZray/RyReflect.git
 */
#pragma once
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <iostream>
#include <concepts>
 // 检查是否定义了RY_USE_QT宏来决定是否使用Qt
#ifdef RY_USE_QT
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#include <QJsonDocument>
#else
#include "RyJson.h"
#endif

namespace RyReflect
{
    template <typename>
    inline constexpr bool always_false = false;

    // 判断类型是否是容器
    template <typename T, typename = void>
    struct is_container : std::false_type
    { };

    template <typename T>
    struct is_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()), typename T::value_type>> : std::true_type
    { };

    // 判断类型 T 是否可以使用 forEach
    template <typename T>
    concept ForEachable = requires(T t) {
        {
            std::remove_cvref_t<T>::getMemberNames()
        };
        {
            t.getMemberValues()
        };
        {
            std::tuple_size_v<decltype(std::remove_cvref_t<T>::getMemberNames())>
        };
    };

    template <typename T, typename F, std::size_t... I>
    void forEachImpl(T&& obj, F&& f, std::index_sequence<I...>)
    {
        using U = std::remove_reference_t<T>;
        auto names = U::getMemberNames();
        auto values = obj.getMemberValues();
        // 通过参数包展开，依次调用传入的函数f
        (f(std::get<I>(names), std::get<I>(values)), ...);
    }

    // 遍历结构体的成员名称和值
    template <ForEachable T, typename F>
    void forEach(T&& obj, F&& f)
    {
        using U = std::remove_reference_t<T>;
        constexpr auto N = std::tuple_size_v<decltype(U::getMemberNames())>;
        forEachImpl(std::forward<T>(obj), std::forward<F>(f), std::make_index_sequence<N>{});
    }

    // 定义辅助宏，将变量名转换为字符串
#define RYREFLECT_STRINGIZE(x) #x
// 展开宏参数，解决宏递归展开问题
#define RYREFLECT_EXPAND(x) x

#define RYREFLECT_FOR_EACH_1(action, x)         action(x)
#define RYREFLECT_FOR_EACH_2(action, x, x1)     action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1))
#define RYREFLECT_FOR_EACH_3(action, x, x1, x2) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2))
#define RYREFLECT_FOR_EACH_4(action, x, x1, x2, x3)                                                                                                                                                    \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3))
#define RYREFLECT_FOR_EACH_5(action, x, x1, x2, x3, x4)                                                                                                                                                \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4))
#define RYREFLECT_FOR_EACH_6(action, x, x1, x2, x3, x4, x5)                                                                                                                                            \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5))
#define RYREFLECT_FOR_EACH_7(action, x, x1, x2, x3, x4, x5, x6)                                                                                                                                        \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6))
#define RYREFLECT_FOR_EACH_8(action, x, x1, x2, x3, x4, x5, x6, x7)                                                                                                                                    \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7))
#define RYREFLECT_FOR_EACH_9(action, x, x1, x2, x3, x4, x5, x6, x7, x8)                                                                                                                                \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8))
#define RYREFLECT_FOR_EACH_10(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9)                                                                                                                           \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9))
#define RYREFLECT_FOR_EACH_11(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)                                                                                                                      \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10))
#define RYREFLECT_FOR_EACH_12(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)                                                                                                                 \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11))
#define RYREFLECT_FOR_EACH_13(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)                                                                                                            \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12))
#define RYREFLECT_FOR_EACH_14(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)                                                                                                       \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13))
#define RYREFLECT_FOR_EACH_15(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)                                                                                                  \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14))
#define RYREFLECT_FOR_EACH_16(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)                                                                                             \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15))
#define RYREFLECT_FOR_EACH_17(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16)                                                                                        \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16))
#define RYREFLECT_FOR_EACH_18(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17)                                                                                   \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17))
#define RYREFLECT_FOR_EACH_19(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18)                                                                              \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18))
#define RYREFLECT_FOR_EACH_20(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19)                                                                         \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19))
#define RYREFLECT_FOR_EACH_21(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20)                                                                    \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20))
#define RYREFLECT_FOR_EACH_22(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21)                                                               \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21))
#define RYREFLECT_FOR_EACH_23(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22)                                                          \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22))
#define RYREFLECT_FOR_EACH_24(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23)                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23))
#define RYREFLECT_FOR_EACH_25(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24)                                                \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24))
#define RYREFLECT_FOR_EACH_26(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25)                                           \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25))
#define RYREFLECT_FOR_EACH_27(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26)                                      \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26))
#define RYREFLECT_FOR_EACH_28(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27)                                 \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27))
#define RYREFLECT_FOR_EACH_29(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28)                            \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28))
#define RYREFLECT_FOR_EACH_30(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29)                       \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29))
#define RYREFLECT_FOR_EACH_31(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30)                  \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30))
#define RYREFLECT_FOR_EACH_32(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31)             \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31))
#define RYREFLECT_FOR_EACH_33(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32)        \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32))
#define RYREFLECT_FOR_EACH_34(action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33)   \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33))
#define RYREFLECT_FOR_EACH_35(                                                                                                                                                                         \
    action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34)                        \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34))
#define RYREFLECT_FOR_EACH_36(                                                                                                                                                                         \
    action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35)                   \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35))
#define RYREFLECT_FOR_EACH_37(                                                                                                                                                                         \
    action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36)              \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36))
#define RYREFLECT_FOR_EACH_38(                                                                                                                                                                         \
    action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37)         \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37))
#define RYREFLECT_FOR_EACH_39(                                                                                                                                                                         \
    action, x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37, x38)    \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38))
#define RYREFLECT_FOR_EACH_40(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39))
#define RYREFLECT_FOR_EACH_41(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40))
#define RYREFLECT_FOR_EACH_42(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41))
#define RYREFLECT_FOR_EACH_43(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42))
#define RYREFLECT_FOR_EACH_44(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43))
#define RYREFLECT_FOR_EACH_45(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44))
#define RYREFLECT_FOR_EACH_46(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45))
#define RYREFLECT_FOR_EACH_47(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46))
#define RYREFLECT_FOR_EACH_48(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47))
#define RYREFLECT_FOR_EACH_49(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48))
#define RYREFLECT_FOR_EACH_50(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49))
#define RYREFLECT_FOR_EACH_51(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50))
#define RYREFLECT_FOR_EACH_52(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51))
#define RYREFLECT_FOR_EACH_53(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52))
#define RYREFLECT_FOR_EACH_54(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53))
#define RYREFLECT_FOR_EACH_55(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54))
#define RYREFLECT_FOR_EACH_56(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55))
#define RYREFLECT_FOR_EACH_57(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56))
#define RYREFLECT_FOR_EACH_58(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57))
#define RYREFLECT_FOR_EACH_59(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58))
#define RYREFLECT_FOR_EACH_60(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58,   x59)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x59))
#define RYREFLECT_FOR_EACH_61(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58,   x59,   x60)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x59)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x60))
#define RYREFLECT_FOR_EACH_62(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58,   x59,   x60,   x61)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x59)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x60)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x61))
#define RYREFLECT_FOR_EACH_63(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58,   x59,   x60,   x61,   x62)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x59)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x60)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x61)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x62))
#define RYREFLECT_FOR_EACH_64(action,x,  x1,    x2,    x3,    x4,    x5,    x6,    x7,    x8,    x9,    x10,   x11,   x12,   x13,   x14,   x15,   x16,   x17,   x18,   x19,   x20,   x21,   x22,   x23,   x24,   x25,   x26,   x27,   x28,   x29,   x30,   x31,   x32,   x33,   x34,   x35,   x36,   x37,   x38,   x39,   x40,   x41,   x42,   x43,   x44,   x45,   x46,   x47,   x48,   x49,   x50,   x51,   x52,   x53,   x54,   x55,   x56,   x57,   x58,   x59,   x60,   x61,   x62,   x63)                                                                                                                                                                     \
    action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x1)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x2)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x3)),                             \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x4)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x5)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x6)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x7)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x8)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x9)),                                    \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x10)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x11)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x12)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x13)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x14)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x15)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x16)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x17)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x18)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x19)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x20)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x21)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x22)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x23)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x24)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x25)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x26)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x27)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x28)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x29)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x30)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x31)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x32)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x33)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x34)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x35)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x36)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x37)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x38)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x39)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x40)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x41)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x42)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x43)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x44)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x45)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x46)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x47)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x48)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x49)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x50)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x51)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x52)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x53)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x54)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x55)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x56)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x57)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x58)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x59)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x60)),                                 \
        RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x61)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x62)), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x63))

// 获取可变参数的数量，选择对应的RYREFLECT_FOR_EACH_N宏
#define RYREFLECT_GET_MACRO(_1, _2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,NAME,                                                                                                                                                                      \
                            ...)                                                                                                                                                                       \
    NAME

// 定义RYREFLECT_FOR_EACH宏，根据参数数量调用对应的宏
#define RYREFLECT_FOR_EACH(action, ...)                                                                                                                                                                \
    RYREFLECT_EXPAND(RYREFLECT_GET_MACRO(__VA_ARGS__,          RYREFLECT_FOR_EACH_64,RYREFLECT_FOR_EACH_63,RYREFLECT_FOR_EACH_62,RYREFLECT_FOR_EACH_61,RYREFLECT_FOR_EACH_60,RYREFLECT_FOR_EACH_59,RYREFLECT_FOR_EACH_58,RYREFLECT_FOR_EACH_57,RYREFLECT_FOR_EACH_56,RYREFLECT_FOR_EACH_55,RYREFLECT_FOR_EACH_54,RYREFLECT_FOR_EACH_53,RYREFLECT_FOR_EACH_52,RYREFLECT_FOR_EACH_51,RYREFLECT_FOR_EACH_50,RYREFLECT_FOR_EACH_49,RYREFLECT_FOR_EACH_48,RYREFLECT_FOR_EACH_47,RYREFLECT_FOR_EACH_46,RYREFLECT_FOR_EACH_45,RYREFLECT_FOR_EACH_44,RYREFLECT_FOR_EACH_43,RYREFLECT_FOR_EACH_42,RYREFLECT_FOR_EACH_41,RYREFLECT_FOR_EACH_40,RYREFLECT_FOR_EACH_39,RYREFLECT_FOR_EACH_38,RYREFLECT_FOR_EACH_37,RYREFLECT_FOR_EACH_36,RYREFLECT_FOR_EACH_35,RYREFLECT_FOR_EACH_34,RYREFLECT_FOR_EACH_33,RYREFLECT_FOR_EACH_32,RYREFLECT_FOR_EACH_31,RYREFLECT_FOR_EACH_30,RYREFLECT_FOR_EACH_29,RYREFLECT_FOR_EACH_28,RYREFLECT_FOR_EACH_27,RYREFLECT_FOR_EACH_26,RYREFLECT_FOR_EACH_25,RYREFLECT_FOR_EACH_24,RYREFLECT_FOR_EACH_23,RYREFLECT_FOR_EACH_22,RYREFLECT_FOR_EACH_21,RYREFLECT_FOR_EACH_20,RYREFLECT_FOR_EACH_19,RYREFLECT_FOR_EACH_18,RYREFLECT_FOR_EACH_17,RYREFLECT_FOR_EACH_16,RYREFLECT_FOR_EACH_15,RYREFLECT_FOR_EACH_14,RYREFLECT_FOR_EACH_13,RYREFLECT_FOR_EACH_12,RYREFLECT_FOR_EACH_11,RYREFLECT_FOR_EACH_10,RYREFLECT_FOR_EACH_9, RYREFLECT_FOR_EACH_8, RYREFLECT_FOR_EACH_7, RYREFLECT_FOR_EACH_6, RYREFLECT_FOR_EACH_5, RYREFLECT_FOR_EACH_4, RYREFLECT_FOR_EACH_3, RYREFLECT_FOR_EACH_2, RYREFLECT_FOR_EACH_1)(action, __VA_ARGS__))

// 定义一个通用的JSON值类型
#ifdef RY_USE_QT
    using JsonValue = QJsonValue;
    using JsonObject = QJsonObject;
    using JsonArray = QJsonArray;
#else
// 如果不使用Qt，这里可以定义自己的JSON类型或使用其他库
    using JsonValue = RyJson;
    using JsonObject = RyJson;
    using JsonArray = RyJsonArray;
#endif

    // 前置声明
    template <typename Container>
    Container fromJsonArray(const JsonArray& jsonArray);

    template <typename Container>
    JsonArray toJsonArray(const Container& container);

    //  将基本类型转换为JsonValue的辅助函数
    template <typename T>
    JsonValue toJsonValue(const T& value)
    {
        if constexpr (std::is_same_v<T, std::string>) {
#ifdef RY_USE_QT
            return QJsonValue(QString::fromStdString(value));
#else
            return JsonValue{ value };
#endif
        }
#ifdef RY_USE_QT
        else if constexpr (std::is_same_v<T, QString>) {
            return value;
        }
        else if constexpr (std::is_same_v<T, QByteArray>) {
            return QString(value);
        }
#endif
        else if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, const char*>) {
#ifdef RY_USE_QT
            return QJsonValue(value);
#else
            return JsonValue{ value };
#endif
        }
        else if constexpr (is_container<T>::value) {
            // 对于容器，调用 toJsonArray
            return toJsonArray(value);
        }
        else if constexpr (ForEachable<T>) {
            // 对于复杂类型，调用其 toJson 方法
            return value.toJson();
        }
        else {
            static_assert(always_false<T>, "Unsupported type in toJsonValue");
        }
    }

    // 从JsonValue转换为基本类型的辅助函数
    template <typename T>
    T fromJsonValue(const JsonValue& jsonValue)
    {
        if constexpr (std::is_same_v<T, std::string>) {
#ifdef RY_USE_QT
            return jsonValue.toString().toStdString();
#else
            return jsonValue.toString();
#endif
        }
#ifdef RY_USE_QT
        else if constexpr (std::is_same_v<T, QString>) {
            return jsonValue.toString();
        }
        else if constexpr (std::is_same_v<T, QByteArray>) {
            return jsonValue.toVariant().toByteArray();
        }
#endif
        else if constexpr (std::is_same_v<T, int>) {
            return jsonValue.toInt32();
        }
        else if constexpr (std::is_same_v<T, double>) {
            return jsonValue.toDouble();
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return jsonValue.toBool();
        }
        else if constexpr (ForEachable<T>) {
            // 对于复杂类型，调用其静态 fromJson 方法
            return T::fromJson(jsonValue.toObject());
        }
        else if constexpr (is_container<T>::value) {
            // 对于容器，调用 fromJsonArray
            return fromJsonArray<T>(jsonValue.toArray());
        }
        else {
            auto typeName = std::string(typeid(T).name());
            static_assert(always_false<T>, "Unsupported type in fromJsonValue: ");
        }
    }

    // 对于 int64_t
    template <>
    inline int64_t fromJsonValue<int64_t>(const JsonValue& value) {
#ifdef RY_USE_QT
        return value.toInteger();
#else
        return value.toInt(); // 根据 JsonValue 的实际实现
#endif
    }

    // 将数组转换为JsonArray的辅助函数
    template <typename Container>
    JsonArray toJsonArray(const Container& container)
    {
        JsonArray jsonArray;
        for (const auto& item : container) {
            jsonArray.push_back(toJsonValue(item));
        }
        return jsonArray;
    }

    // 从JsonArray转换为容器的辅助函数
    template <typename Container>
    Container fromJsonArray(const JsonArray& jsonArray)
    {
        using T = typename Container::value_type;
        Container container;
        for (const auto& jsonValue : jsonArray) {
            container.insert(container.end(), fromJsonValue<T>(jsonValue));
        }
        return container;
    }

    // 定义RY_REFLECTABLE宏，用于在结构体中声明反射所需的成员函数
#define RY_REFLECTABLE(TypeName, ...)                                                                                                                                                                  \
    auto getMemberValues()                                                                                                                                                                             \
    {                                                                                                                                                                                                  \
        return std::tie(__VA_ARGS__);                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
    auto getMemberValues() const                                                                                                                                                                       \
    {                                                                                                                                                                                                  \
        return std::tie(__VA_ARGS__);                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
    constexpr static auto getMemberNames()                                                                                                                                                             \
    {                                                                                                                                                                                                  \
        return std::make_tuple(RYREFLECT_FOR_EACH(RYREFLECT_STRINGIZE, __VA_ARGS__));                                                                                                                  \
    }                                                                                                                                                                                                  \
    RyReflect::JsonObject toJson() const                                                                                                                                                               \
    {                                                                                                                                                                                                  \
        RyReflect::JsonObject json;                                                                                                                                                                    \
        try {                                                                                                                                                                                          \
            RyReflect::forEach(*this, [&json](const auto& name, const auto& value) {                                                                                                                   \
                json[name] = RyReflect::toJsonValue(value);                                                                                                                                            \
            });                                                                                                                                                                                        \
        }                                                                                                                                                                                              \
        catch (const std::exception& e) {                                                                                                                                                              \
            std::cerr << "Error in toJson: " << e.what() << std::endl;                                                                                                                                 \
            throw;                                                                                                                                                                                     \
        }                                                                                                                                                                                              \
        return json;                                                                                                                                                                                   \
    }                                                                                                                                                                                                  \
    static TypeName fromJson(const RyReflect::JsonObject& json)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        TypeName obj;                                                                                                                                                                                  \
        try {                                                                                                                                                                                          \
            RyReflect::forEach(obj, [&json](const auto& name, auto& value) {                                                                                                                           \
                if (json.contains(name)) {\
                    value = RyReflect::fromJsonValue<std::remove_reference_t<decltype(value)>>(json.value(name));                                                                                      \
                }                                                                                                                                                                                      \
                else {                                                                                                                                                                                 \
                    std::cerr << "Warning: Key '" << name << "' not found in JSON" << std::endl;                                                                                                       \
                }                                                                                                                                                                                      \
            });                                                                                                                                                                                        \
        }                                                                                                                                                                                              \
        catch (const std::exception& e) {                                                                                                                                                              \
            std::cerr << "Error in fromJson: " << e.what() << std::endl;                                                                                                                               \
            throw;                                                                                                                                                                                     \
        }                                                                                                                                                                                              \
        return obj;                                                                                                                                                                                    \
    }
    // 如果你不用Qt，那么可以修改fromJson的json.value为你的json实现
} // namespace RyReflect
