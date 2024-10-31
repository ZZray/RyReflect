/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 测试
 */
#include "RyReflect.h"
#include <string>
#include <iostream>

void testForEach()
{
    struct User
    {
        std::string name;
        double age;

        RY_REFLECTABLE(User, name, age)
    };
    User user{.name = "Ray", .age = 20};
    // 遍历成员并输出
    RyReflect::forEach(user, []<typename T0>(const auto& name, T0& value) {
        std::cout << name << " = " << value << std::endl;
        if (name == "age") {
            if constexpr (std::is_same_v<std::decay_t<T0>, double>) {
                value = 21; // 修改成员值
            }
        }
        else if (name == "name") {
            if constexpr (std::is_same_v<std::decay_t<T0>, std::string>) {
                value = "Ray2"; // 修改成员值
            }
        }
    });

    std::cout << "user.age = " << user.age << std::endl;
}

void testJson()
{
    struct Address
    {
        std::string street;
        std::string city;

        RY_REFLECTABLE(Address, street, city)
    };

    struct User
    {
        std::string name;
        int age;
        bool isActive;
        Address address;

        RY_REFLECTABLE(User, name, age, isActive, address)
    };
    User user{.name = "Ray", .age = 30, .isActive = true, .address = Address{.street = "123 Main St", .city = "Anytown"}};

    // 转换为JSON
    QJsonObject jsonObj = user.toJson();

#ifdef RY_USE_QT
    qDebug() << "JSON:" << jsonObj;
#else
    std::cout << "JSON created (output format depends on your JsonObject implementation)" << std::endl;
#endif

    // 从JSON创建新对象
    User newUser = User::fromJson(jsonObj);

#ifdef RY_USE_QT
    qDebug() << "Reconstructed user:" << QString::fromStdString(newUser.name) << newUser.age << newUser.isActive;
    qDebug() << "Address:" << QString::fromStdString(newUser.address.street) << QString::fromStdString(newUser.address.city);
#else
    std::cout << "Reconstructed user: " << newUser.name << " " << newUser.age << " " << newUser.isActive << std::endl;
    std::cout << "Address: " << newUser.address.street << ", " << newUser.address.city << std::endl;
#endif
}
int main(int argc, char* argv[])
{
    testJson();
    return 0;
}