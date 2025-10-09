#pragma once
#include <vector>
#include <unordered_map>
#include <string>

//����
#define SAMEHAND 1
//����
#define ANOTHERHAND 0


#define ex_smalltu  (540001)
#define ex_nn       (540002)
#define ex_youon    (540003)
#define ex_boin (100008)
#define ex_sakujo (540004)

using  KeyCode = unsigned int;
struct KMP {
    enum class Dir {
        None, L, R,
    };
    struct KMPelm {
        enum class Cmd
        {
            Send,
            DSend
        };

        Cmd cmd;
        KMP::Dir dir;
        KeyCode to;
        KeyCode tootherdir;//�Ⴄ��ŉ�������
        KeyCode secboin;//��x�ڂ̕ꉹ
    };

    std::unordered_map<KeyCode, KMPelm>KMPs;
    std::unordered_map<std::string, KeyCode>ids;
    std::unordered_map<std::string, Dir>dirs;
};

