# RyReflect

RyReflect 是一个基于现代 C++ 的轻量级反射库，支持自动生成结构体的 JSON 序列化和反序列化，兼容 Qt 和非 Qt 环境。

## 特性

- **现代 C++**：利用 C++20/23 的特性，如 Concepts、constexpr、std::tuple 等。
- **自动生成**：通过宏定义，自动为结构体生成 `toJson` 和 `fromJson` 方法。
- **Qt 支持**：可选支持 Qt 的 JSON 库，兼容 `QJsonObject`、`QJsonValue` 等。
- **嵌套类型**：支持嵌套结构体的序列化和反序列化。
- **灵活配置**：通过 CMake 选项，选择性地启用或禁用 Qt 支持。

## 依赖

- **编译器**：支持 C++23 的编译器（如 GCC 13，Clang 16，MSVC 2022）
- **CMake**：3.10 或更高版本
- **Qt**（可选）：Qt 5.15 或 Qt 6

## 安装

1. **克隆仓库**

   ```bash
   git clone https://github.com/yourusername/RyReflect.git
   cd RyReflect
   ```

2. **配置项目**

   - **不使用 Qt**

     ```bash
     cmake -B build
     ```

   - **使用 Qt**

     ```bash
     cmake -B build -DUSE_QT=ON
     ```

3. **编译项目**

   ```bash
   cmake --build build
   ```

## 使用方法

### 定义可反射的结构体

使用 `RY_REFLECTABLE` 宏为结构体添加反射功能，需传入类型名和成员变量列表：

```cpp
#include "RyReflect.h"

struct Address {
    std::string m_street;
    std::string m_city;

    RY_REFLECTABLE(Address, m_street, m_city)
};

struct User {
    std::string m_name;
    int m_age;
    bool m_isActive;
    Address m_address;

    RY_REFLECTABLE(User, m_name, m_age, m_isActive, m_address)
};
```

### 序列化和反序列化

```cpp
int main() {
    User user{
        .m_name = "Ray",
        .m_age = 30,
        .m_isActive = true,
        .m_address = Address{.m_street = "123 Main St", .m_city = "Anytown"}
    };

    // 序列化为 JSON
    RyReflect::JsonObject jsonObj = user.toJson();

    // 输出 JSON
    #ifdef RY_USE_QT
    qDebug() << "JSON:" << jsonObj;
    #else
    // 非 Qt 环境，请根据您的 JsonObject 实现自行输出
    #endif

    // 从 JSON 反序列化
    User newUser = User::fromJson(jsonObj);

    // 输出新对象的数据
    #ifdef RY_USE_QT
    qDebug() << "Reconstructed user:" << QString::fromStdString(newUser.m_name)
             << newUser.m_age << newUser.m_isActive;
    qDebug() << "Address:" << QString::fromStdString(newUser.m_address.m_street)
             << QString::fromStdString(newUser.m_address.m_city);
    #else
    // 非 Qt 环境，输出 newUser 的数据
    #endif

    return 0;
}
```

## 配置选项

- `USE_QT`（默认：`OFF`）：是否启用 Qt 支持。

  - 启用方式：

    ```bash
    cmake -B build -DUSE_QT=ON
    ```

## 代码结构

- `RyReflect.h`：主要的反射实现，包括宏定义和模板函数。
- `main.cpp`：示例代码，演示如何使用 RyReflect 进行序列化和反序列化。

## 注意事项

- **非 Qt 环境**：如果不使用 Qt，需要自行实现 `JsonValue`、`JsonObject` 和 `JsonArray`，或者使用第三方 JSON 库（如 [nlohmann/json](https://github.com/nlohmann/json)），并调整 `toJsonValue` 和 `fromJsonValue` 函数。
- **类型支持**：`toJsonValue` 和 `fromJsonValue` 函数目前支持基本类型和具有 `RY_REFLECTABLE` 宏定义的复杂类型。对于其他类型（如容器、指针等），需要扩展这些函数。
- **错误处理**：在 `toJson` 和 `fromJson` 方法中添加了异常捕获和错误信息输出，方便调试。
- **成员变量命名**：建议遵循小驼峰式命名，成员变量以 `m_` 开头。

## 扩展

- **支持更多类型**：可以扩展 `toJsonValue` 和 `fromJsonValue`，支持 `std::vector`、`std::map` 等容器类型。
- **优化宏定义**：根据实际需求，优化 `RY_REFLECTABLE` 宏，支持更多的成员变量或特殊情况。
- **自定义错误处理**：根据项目需要，调整错误处理方式，如在反序列化时抛出异常或提供默认值。

## 贡献

欢迎提交 issue 和 pull request 来改进 RyReflect。如果您有任何建议或发现了问题，请与我们联系。

## 许可证

[MIT License](LICENSE)