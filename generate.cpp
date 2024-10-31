/**
 * @author rayzhang
 * @date 2024��10��31��
 * @description �������ɴ���
 */
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

  // ��ȡ��ǰ�����ַ����ĺ���
std::string getCurrentDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y��%m��%d��", now);
    return std::string(buf);
}

int main() {
    int N;

    // ��ʾ�û�������Ҫ���ɵĺ궨������
    std::cout << "������Ҫ֧�ֵ�����������������64����";
    std::cin >> N;

    // ����������Ч��
    if (N <= 0) {
        std::cerr << "������һ����������" << std::endl;
        return 1;
    }

    // ������ļ�
    std::ofstream outfile("ryreflect_macros.h");

    // ����ļ��Ƿ�ɹ���
    if (!outfile.is_open()) {
        std::cerr << "�޷�������ļ���" << std::endl;
        return 1;
    }

    // д���ļ�ͷ��Ϣ
    outfile << "/**\n";
    outfile << " * @author rayzhang\n";
    outfile << " * @date " << getCurrentDate() << "\n";
    outfile << " * @description ʵ��\n";
    outfile << " */\n\n";

    // �����ظ�����
    outfile << "#ifndef RYREFLECT_MACROS_H\n";
    outfile << "#define RYREFLECT_MACROS_H\n\n";

    // д���Ҫ�ĺ궨��
    outfile << "// ������ת��Ϊ�ַ���\n";
    outfile << "#define RYREFLECT_STRINGIZE(x) #x\n";
    outfile << "// չ�������\n";
    outfile << "#define RYREFLECT_EXPAND(x) x\n";
    outfile << "// ����������ʶ��\n";
    outfile << "#define RYREFLECT_CONCAT(a, b) a##b\n\n";

    // д��RYREFLECT_RSEQ_N�꣬���ڲ�������
    outfile << "// �������У����ڲ�������\n";
    outfile << "#define RYREFLECT_RSEQ_N() \\\n";
    for (int i = N + 1; i >= 0; --i) {
        outfile << "    " << i << (i > 0 ? "," : "") << (i % 10 == 0 ? " \\\n" : "");
    }
    outfile << "\n\n";

    // д��RYREFLECT_COUNT_ARGS_IMPL��
    outfile << "// ��ȡ����������ʵ�ֺ�\n";
    outfile << "#define RYREFLECT_COUNT_ARGS_IMPL(";
    for (int i = 0; i <= N + 1; ++i) {
        outfile << "_" << i;
        if (i < N + 1) {
            outfile << ",";
        }
    }
    outfile << "N,...) N\n\n";

    // д��RYREFLECT_COUNT_ARGS��
    outfile << "// ��ȡ���������ĺ�\n";
    outfile << "#define RYREFLECT_COUNT_ARGS(...) \\\n";
    outfile << "    RYREFLECT_EXPAND(RYREFLECT_COUNT_ARGS_IMPL(0, ## __VA_ARGS__, RYREFLECT_RSEQ_N()))\n\n";

    // д��RYREFLECT_FOR_EACH��
    outfile << "// RYREFLECT_FOR_EACH�꣬���ݲ����������ö�Ӧ��RYREFLECT_FOR_EACH_N��\n";
    outfile << "#define RYREFLECT_FOR_EACH(action, ...) \\\n";
    outfile << "    RYREFLECT_EXPAND(RYREFLECT_CONCAT(RYREFLECT_FOR_EACH_, RYREFLECT_COUNT_ARGS(__VA_ARGS__)) (action, __VA_ARGS__))\n\n";

    // ����RYREFLECT_FOR_EACH_N�궨��
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

    // д��RYREFLECT_REFLECTABLE��
    outfile << "// ����RYREFLECT_REFLECTABLE�꣬�����ڽṹ����������������ĳ�Ա����\n";
    outfile << "#define RYREFLECT_REFLECTABLE(...) \\\n";
    outfile << "    auto getMemberValues() { \\\n";
    outfile << "        return std::tie(__VA_ARGS__); \\\n";
    outfile << "    } \\\n";
    outfile << "    constexpr static auto getMemberNames() { \\\n";
    outfile << "        return std::make_tuple(RYREFLECT_FOR_EACH(RYREFLECT_STRINGIZE, __VA_ARGS__)); \\\n";
    outfile << "    }\n\n";

    // ����ifndef
    outfile << "#endif // RYREFLECT_MACROS_H\n";

    // �ر��ļ�
    outfile.close();

    std::cout << "�궨�������ɲ����浽 ryreflect_macros.h �ļ��С�" << std::endl;

    return 0;
}
