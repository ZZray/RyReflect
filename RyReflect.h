/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 实现
 */
#pragma once
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <concepts>
// 检查是否定义了RY_USE_QT宏来决定是否使用Qt
#ifdef RY_USE_QT
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#endif

namespace RyReflect
{
template <typename>
inline constexpr bool always_false = false;

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
    using U     = std::remove_reference_t<T>;
    auto names  = U::getMemberNames();
    auto values = obj.getMemberValues();
    // 通过参数包展开，依次调用传入的函数f
    (f(std::get<I>(names), std::get<I>(values)), ...);
}

// 遍历结构体的成员名称和值
template <ForEachable T, typename F>
void forEach(T&& obj, F&& f)
{
    using U          = std::remove_reference_t<T>;
    constexpr auto N = std::tuple_size_v<decltype(U::getMemberNames())>;
    forEachImpl(std::forward<T>(obj), std::forward<F>(f), std::make_index_sequence<N>{});
}

// 定义辅助宏，将变量名转换为字符串
#define RYREFLECT_STRINGIZE(x) #x
// 展开宏参数，解决宏递归展开问题
#define RYREFLECT_EXPAND(x) x

// 定义RYREFLECT_FOR_EACH宏，用于对每个参数应用指定的操作
#define RYREFLECT_FOR_EACH_1(action, x)      action(x)
#define RYREFLECT_FOR_EACH_2(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_3(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_2(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_4(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_3(action, __VA_ARGS__))
#define RYREFLECT_FOR_EACH_5(action, x, ...) action(x), RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_4(action, __VA_ARGS__))

// 获取可变参数的数量，选择对应的RYREFLECT_FOR_EACH_N宏
#define RYREFLECT_GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

// 定义RYREFLECT_FOR_EACH宏，根据参数数量调用对应的宏
#define RYREFLECT_FOR_EACH(action, ...)                                                                                                                                                                \
    RYREFLECT_EXPAND(RYREFLECT_GET_MACRO(__VA_ARGS__, RYREFLECT_FOR_EACH_5, RYREFLECT_FOR_EACH_4, RYREFLECT_FOR_EACH_3, RYREFLECT_FOR_EACH_2, RYREFLECT_FOR_EACH_1)(action, __VA_ARGS__))

// 定义一个通用的JSON值类型
#ifdef RY_USE_QT
using JsonValue  = QJsonValue;
using JsonObject = QJsonObject;
using JsonArray  = QJsonArray;
#else
// 如果不使用Qt，这里可以定义自己的JSON类型或使用其他库
struct JsonValue
{
    std::variant<std::nullptr_t, bool, int, double, std::string, std::vector<JsonValue>, std::map<std::string, JsonValue>> value;
};
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray  = std::vector<JsonValue>;
#endif

//  将基本类型转换为JsonValue的辅助函数
template <typename T>
JsonValue toJsonValue(const T& value)
{
    if constexpr (std::is_same_v<T, std::string>) {
#ifdef RY_USE_QT
        return QJsonValue(QString::fromStdString(value));
#else
        return JsonValue{value};
#endif
    }
    else if constexpr (std::is_arithmetic_v<T>) {
#ifdef RY_USE_QT
        return QJsonValue(value);
#else
        return JsonValue{value};
#endif
    }
    else if constexpr (ForEachable<T>) {
        // 对于复杂类型，调用其 toJson 方法
        return value.toJson();
    }
    else {
        static_assert(always_false<T>, "Unsupported type in toJsonValue");
    }
}

// 新增：从JsonValue转换为基本类型的辅助函数
template <typename T>
T fromJsonValue(const JsonValue& jsonValue)
{
    if constexpr (std::is_same_v<T, std::string>) {
#ifdef RY_USE_QT
        return jsonValue.toString().toStdString();
#else
        return std::get<std::string>(jsonValue.value);
#endif
    }
    else if constexpr (std::is_same_v<T, int>) {
#ifdef RY_USE_QT
        return jsonValue.toInt();
#else
        return std::get<int>(jsonValue.value);
#endif
    }
    else if constexpr (std::is_same_v<T, double>) {
#ifdef RY_USE_QT
        return jsonValue.toDouble();
#else
        return std::get<double>(jsonValue.value);
#endif
    }
    else if constexpr (std::is_same_v<T, bool>) {
#ifdef RY_USE_QT
        return jsonValue.toBool();
#else
        return std::get<bool>(jsonValue.value);
#endif
    }
    else if constexpr (ForEachable<T>) {
        // 对于复杂类型，调用其静态 fromJson 方法
#ifdef RY_USE_QT
        return T::fromJson(jsonValue.toObject());
#else
        return T::fromJson(std::get<JsonObject>(jsonValue.value));
#endif
    }
    else {
        static_assert(always_false<T>, "Unsupported type in fromJsonValue");
    }
}
// 定义RYREFLECT_REFLECTABLE宏，用于在结构体中声明反射所需的成员函数
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
                if (json.contains(name)) {                                                                                                                                                             \
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

} // namespace RyReflect