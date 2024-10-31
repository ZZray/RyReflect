/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 测试
 */
#include "RyReflect.h"
#include <string>
#include <iostream>

struct User {
	std::string name;
	double age;
	RY_REFLECTABLE(name, age)
};

int main(int argc, char* argv[])
{
	User user{ .name = "Ray", .age = 20 };
    // 遍历成员并输出
	RyReflect::forEach(user, []<typename T0>(const auto & name, T0 & value) {
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
	return 0;
}