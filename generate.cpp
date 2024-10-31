/**
 * @author rayzhang
 * @date 2024年10月31日
 * @description 用来生成代码
 */
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

  // 获取当前日期字符串的函数
std::string getCurrentDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y年%m月%d日", now);
    return std::string(buf);
}

int main() {
    int N;

    // 提示用户输入需要生成的宏定义数量
    std::cout << "请输入要支持的最大参数数量（例如64）：";
    std::cin >> N;

    // 检查输入的有效性
    if (N <= 0) {
        std::cerr << "请输入一个正整数！" << std::endl;
        return 1;
    }

    // 打开输出文件
    std::ofstream outfile("ryreflect_macros.h");

    // 检查文件是否成功打开
    if (!outfile.is_open()) {
        std::cerr << "无法打开输出文件！" << std::endl;
        return 1;
    }

    // 写入文件头信息
    outfile << "/**\n";
    outfile << " * @author rayzhang\n";
    outfile << " * @date " << getCurrentDate() << "\n";
    outfile << " * @description 实现\n";
    outfile << " */\n\n";

    // 避免重复包含
    outfile << "#ifndef RYREFLECT_MACROS_H\n";
    outfile << "#define RYREFLECT_MACROS_H\n\n";

    // 写入必要的宏定义
    outfile << "// 将参数转换为字符串\n";
    outfile << "#define RYREFLECT_STRINGIZE(x) #x\n";
    outfile << "// 展开宏参数\n";
    outfile << "#define RYREFLECT_EXPAND(x) x\n";
    outfile << "// 连接两个标识符\n";
    outfile << "#define RYREFLECT_CONCAT(a, b) a##b\n\n";

    // 写入RYREFLECT_RSEQ_N宏，用于参数计数
    outfile << "// 反向序列，用于参数计数\n";
    outfile << "#define RYREFLECT_RSEQ_N() \\\n";
    for (int i = N + 1; i >= 0; --i) {
        outfile << "    " << i << (i > 0 ? "," : "") << (i % 10 == 0 ? " \\\n" : "");
    }
    outfile << "\n\n";

    // 写入RYREFLECT_COUNT_ARGS_IMPL宏
    outfile << "// 获取参数数量的实现宏\n";
    outfile << "#define RYREFLECT_COUNT_ARGS_IMPL(";
    for (int i = 0; i <= N + 1; ++i) {
        outfile << "_" << i;
        if (i < N + 1) {
            outfile << ",";
        }
    }
    outfile << "N,...) N\n\n";

    // 写入RYREFLECT_COUNT_ARGS宏
    outfile << "// 获取参数数量的宏\n";
    outfile << "#define RYREFLECT_COUNT_ARGS(...) \\\n";
    outfile << "    RYREFLECT_EXPAND(RYREFLECT_COUNT_ARGS_IMPL(0, ## __VA_ARGS__, RYREFLECT_RSEQ_N()))\n\n";

    // 写入RYREFLECT_FOR_EACH宏
    outfile << "// RYREFLECT_FOR_EACH宏，根据参数数量调用对应的RYREFLECT_FOR_EACH_N宏\n";
    outfile << "#define RYREFLECT_FOR_EACH(action, ...) \\\n";
    outfile << "    RYREFLECT_EXPAND(RYREFLECT_CONCAT(RYREFLECT_FOR_EACH_, RYREFLECT_COUNT_ARGS(__VA_ARGS__)) (action, __VA_ARGS__))\n\n";

    // 生成RYREFLECT_FOR_EACH_N宏定义
    for (int i = 1; i <= N; ++i) {
        if (i == 1) {
            outfile << "#define RYREFLECT_FOR_EACH_" << i << "(action, x1) \\\n";
            outfile << "    action(x1)\n\n";
        }
        else {
            outfile << "#define RYREFLECT_FOR_EACH_" << i << "(action, x1, ...) \\\n";
            outfile << "    action(x1), RYREFLECT_FOR_EACH_" << (i - 1) << "(action, __VA_ARGS__)\n\n";
        }
    }

    // 写入RYREFLECT_REFLECTABLE宏
    outfile << "// 定义RYREFLECT_REFLECTABLE宏，用于在结构体中声明反射所需的成员函数\n";
    outfile << "#define RYREFLECT_REFLECTABLE(...) \\\n";
    outfile << "    auto getMemberValues() { \\\n";
    outfile << "        return std::tie(__VA_ARGS__); \\\n";
    outfile << "    } \\\n";
    outfile << "    constexpr static auto getMemberNames() { \\\n";
    outfile << "        return std::make_tuple(RYREFLECT_FOR_EACH(RYREFLECT_STRINGIZE, __VA_ARGS__)); \\\n";
    outfile << "    }\n\n";

    // 结束ifndef
    outfile << "#endif // RYREFLECT_MACROS_H\n";

    // 关闭文件
    outfile.close();

    std::cout << "宏定义已生成并保存到 ryreflect_macros.h 文件中。" << std::endl;

    return 0;
}
