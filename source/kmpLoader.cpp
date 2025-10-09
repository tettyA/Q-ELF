#include "kmpLoader.h"


#include <algorithm>

void split(std::string string, char separator, std::vector<std::string>& list) {
    int separator_length = 1;

    list = std::vector<std::string>();

    auto offset = std::string::size_type(0);
    while (1) {
        auto pos = string.find(separator, offset);
        if (pos == std::string::npos) {
            list.push_back(string.substr(offset));
            break;
        }
        list.push_back(string.substr(offset, pos - offset));
        offset = pos + separator_length;
    }
}

void Loadkmp(std::string filepath, KMP& kmp) {
    std::locale::global(std::locale("japanese"));


    std::ifstream kmpfile(filepath);
    kmpfile.imbue(std::locale("japanese"));
    enum CMode
    {
        def, keymap
    };

    CMode currentmode = def;

    std::string line;
    while (std::getline(kmpfile, line)) {


        auto newwend = std::remove_if(line.begin(), line.end(), ::isspace);
        line.erase(newwend, line.end());

        if (line[0] == '-')continue;
        if (line == "")continue;

        if (line[0] == '@') {
            line = line.substr(1);

            if (line == "def") {
                currentmode = def;
            }
            else if (line == "keymap") {
                currentmode = keymap;
            }
            continue;
        }

        if (line[0] == '#' && currentmode == def) {
            line = line.substr(1);
            std::vector<std::string>sepl;
            split(line, ':', sepl);

            if (sepl[0] == "!iddef") {
                kmp.ids[sepl[1]] = std::stoi(sepl[2], nullptr, 0);
                kmp.dirs[sepl[1]] = KMP::Dir::None;
            }
            else if (sepl[0] == "dirdef") {
                if (sepl[2] == "R") kmp.dirs[sepl[1]] = KMP::Dir::R;
                else if (sepl[2] == "L") kmp.dirs[sepl[1]] = KMP::Dir::L;

            }
            continue;
        }

        if (currentmode == keymap) {
            std::vector<std::string>sepl;
            split(line, ':', sepl);

            KMP::KMPelm telm;
            KeyCode from = 0;

            telm.dir = kmp.dirs[sepl[0]];

            if (sepl[1] == "Send") {
                telm.cmd = KMP::KMPelm::Cmd::Send;
                telm.to = kmp.ids[sepl[2]];
            }
            else if (sepl[1] == "DSend") {
                telm.cmd = KMP::KMPelm::Cmd::DSend;

                std::vector<std::string>seplfs;
                split(sepl[2], '-', seplfs);

                std::vector<std::string>seplfrl;
                split(seplfs[0], '/', seplfrl);

                telm.to = kmp.ids[seplfrl[0]];

                if (seplfrl.size() >= 2) telm.tootherdir = kmp.ids[seplfrl[1]];
                else telm.tootherdir = kmp.ids[seplfrl[0]];

                telm.secboin = kmp.ids[seplfs[1]];
            }

            if (kmp.ids[sepl[0]] == kmp.ids[sepl[2]])continue;

            kmp.KMPs[kmp.ids[sepl[0]]] = telm;
        }
    }
    kmpfile.close();
    return;
}
