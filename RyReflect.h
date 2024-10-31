/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 实现
 */
#pragma once
#include <utility>
namespace RyReflect {

	// 定义辅助宏，将变量名转换为字符串
#define RYREFLECT_STRINGIZE(x) #x
// 展开宏参数，解决宏递归展开问题
#define RYREFLECT_EXPAND(x) x

// 定义RYREFLECT_FOR_EACH宏，用于对每个参数应用指定的操作
#define RYREFLECT_FOR_EACH_1(action, x) action(x)
#define RYREFLECT_FOR_EACH_2(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_3(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_2(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_4(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_3(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_5(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_4(action, __VA_ARGS__))

// 获取可变参数的数量，选择对应的RYREFLECT_FOR_EACH_N宏
#define RYREFLECT_GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME

// 定义RYREFLECT_FOR_EACH宏，根据参数数量调用对应的宏
#define RYREFLECT_FOR_EACH(action, ...) \
  RYREFLECT_EXPAND(RYREFLECT_GET_MACRO(__VA_ARGS__, RYREFLECT_FOR_EACH_5, RYREFLECT_FOR_EACH_4, RYREFLECT_FOR_EACH_3, RYREFLECT_FOR_EACH_2, RYREFLECT_FOR_EACH_1)(action, __VA_ARGS__))

// 定义RYREFLECT_REFLECTABLE宏，用于在结构体中声明反射所需的成员函数
#define RY_REFLECTABLE(...) \
    auto getMemberValues() { \
        return std::tie(__VA_ARGS__); \
    } \
    auto getMemberValues() const { \
        return std::tie(__VA_ARGS__); \
    } \
    constexpr static auto getMemberNames() { \
        return std::make_tuple(RYREFLECT_FOR_EACH(RYREFLECT_STRINGIZE, __VA_ARGS__)); \
    }

	// 判断类型 T 是否可以使用 forEach 
    template<typename T>
    concept ForEachable = requires(T t) {
        { std::remove_cvref_t<T>::getMemberNames() } ;
        { t.getMemberValues() } ;
        { std::tuple_size_v<decltype(std::remove_cvref_t<T>::getMemberNames())> };
    };

    template<typename T, typename F, std::size_t... I>
    void forEachImpl(T&& obj, F&& f, std::index_sequence<I...>) {
        using U = std::remove_reference_t<T>;
        auto names = U::getMemberNames();
        auto values = obj.getMemberValues();
        // 通过参数包展开，依次调用传入的函数f
        (f(std::get<I>(names), std::get<I>(values)), ...);
    }

    // 遍历结构体的成员名称和值
    template<ForEachable T, typename F>
    void forEach(T&& obj, F&& f) {
        using U = std::remove_reference_t<T>;
        constexpr auto N = std::tuple_size_v<decltype(U::getMemberNames())>;
        forEachImpl(std::forward<T>(obj), std::forward<F>(f), std::make_index_sequence<N>{});
    }

}