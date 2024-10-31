/**
 * @author rayzhang
 * @date 2024��10��31��
 * @description ����
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
    // ������Ա�����
	RyReflect::forEach(user, []<typename T0>(const auto & name, T0 & value) {
        std::cout << name << " = " << value << std::endl;
        if (name == "age") {
            if constexpr (std::is_same_v<std::decay_t<T0>, double>) {
                value = 21; // �޸ĳ�Աֵ
            }
        }
        else if (name == "name") {
            if constexpr (std::is_same_v<std::decay_t<T0>, std::string>) {
                value = "Ray2"; // �޸ĳ�Աֵ
            }
        }
    });

	std::cout << "user.age = " << user.age << std::endl;
	return 0;
}