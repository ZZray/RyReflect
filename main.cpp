/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 测试
 */
#include "RyReflect.h"
#include <string>
#include <iostream>
#include <set>
#include <list>

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
    struct Person
    {
        std::string name;
        int age;

        RY_REFLECTABLE(Person, name, age)
    };

    // 定义另一个包含容器的结构体
    struct Team
    {
        std::string teamName;
        std::vector<Person> memberVector;
        std::list<Person> memberList;
        std::vector<std::string> memberNames;

        RY_REFLECTABLE(Team, teamName, memberVector, memberList, memberNames)
    };
    // 创建测试数据
    Person person1{ "Alice", 30 };
    Person person2{ "Bob", 25 };
    std::vector personVector{ person1, person2 };
    std::list personList{ person1, person2 };
    std::vector<std::string> nameSet{ "Alice", "Bob" };

    Team team{ "Developers", personVector, personList, nameSet };

    // 序列化为Json
    RyReflect::JsonObject teamJson = team.toJson();

    // 输出序列化结果
    std::cout << "Serialized Json:" << std::endl;
#ifdef RY_USE_QT
    QJsonDocument doc(teamJson);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    std::cout << jsonString.toStdString() << std::endl;
#else
    // todo: implement your json output
#endif

    // 反序列化回对象
    Team newTeam = Team::fromJson(teamJson);

    // 输出反序列化结果
    std::cout << "Deserialized Object:" << std::endl;
    std::cout << "Team Name: " << newTeam.teamName << std::endl;

    std::cout << "Member Vector:" << std::endl;
    for (const auto& member : newTeam.memberVector) {
        std::cout << "Member Name: " << member.name << ", Age: " << member.age << std::endl;
    }

    std::cout << "Member List:" << std::endl;
    for (const auto& member : newTeam.memberList) {
        std::cout << "Member Name: " << member.name << ", Age: " << member.age << std::endl;
    }

    std::cout << "Member Names Set:" << std::endl;
    for (const auto& name : newTeam.memberNames) {
        std::cout << "Member Name: " << name << std::endl;
    }

}
int main(int argc, char* argv[])
{
    testJson();
    return 0;
}