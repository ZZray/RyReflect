/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 生成代码文件的程序
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// 保留的文件头部内容
const std::string header = R"(

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

// 判断类型是否是容器
template <typename T, typename = void>
struct is_container : std::false_type
{
};

template <typename T>
struct is_container<
    T,
    std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    typename T::value_type>> : std::true_type
{
};

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

)";

// 主程序
int main()
{
    int paramCount;
    std::cout << "请输入需要支持的最大参数数量：";
    std::cin >> paramCount;

    // 生成 RYREFLECT_FOR_EACH_N 宏
    std::ostringstream macroStream;
    for (int i = 1; i <= paramCount; ++i) {
        macroStream << "#define RYREFLECT_FOR_EACH_" << i << "(action, x";
        for (int j = 1; j < i; ++j) {
            macroStream << ", x" << j;
        }
        macroStream << ") action(x)";
        for (int j = 1; j < i; ++j) {
            macroStream << ", RYREFLECT_EXPAND(RYREFLECT_FOR_EACH_1(action, x" << j << "))";
        }
        macroStream << "\n";
    }

    // 生成 RYREFLECT_GET_MACRO 和 RYREFLECT_FOR_EACH 宏
    macroStream << "\n// 获取可变参数的数量，选择对应的RYREFLECT_FOR_EACH_N宏\n";
    macroStream << "#define RYREFLECT_GET_MACRO(_1";
    for (int i = 2; i <= paramCount; ++i) {
        macroStream << ", _" << i;
    }
    macroStream << ", NAME, ...) NAME\n\n";

    macroStream << "// 定义RYREFLECT_FOR_EACH宏，根据参数数量调用对应的宏\n";
    macroStream << "#define RYREFLECT_FOR_EACH(action, ...) \\\n";
    macroStream << "    RYREFLECT_EXPAND(RYREFLECT_GET_MACRO(__VA_ARGS__";
    for (int i = paramCount; i >= 1; --i) {
        macroStream << ", RYREFLECT_FOR_EACH_" << i;
    }
    macroStream << ")(action, __VA_ARGS__))\n\n";

    // 将生成的宏与文件头部内容合并
    std::string fullContent = header + macroStream.str();

    // 将剩余的代码内容追加
    fullContent += R"(
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
    else if constexpr (is_container<T>::value) {
        // 对于容器，调用 fromJsonArray
#ifdef RY_USE_QT
        return fromJsonArray<T>(jsonValue.toArray());
#else
        return fromJsonArray<T>(std::get<JsonArray>(jsonValue.value));
#endif
    }
    else {
        static_assert(always_false<T>, "Unsupported type in fromJsonValue");
    }
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


)";

    // 将生成的内容写入文件
    std::ofstream outFile("RyReflect.h");
    if (outFile.is_open()) {
        outFile << fullContent;
        outFile.close();
        std::cout << "代码文件已生成：RyReflect.h" << std::endl;
    }
    else {
        std::cerr << "无法创建文件！" << std::endl;
    }

    return 0;
}
