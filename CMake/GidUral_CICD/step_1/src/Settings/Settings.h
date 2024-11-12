#ifndef GIDURALCLIENT_SETTINGS_H
#define GIDURALCLIENT_SETTINGS_H

#include <vector>
#include <iostream>
#include "dotenv.h"

using namespace std;
using namespace dotenv;

struct Settings {
    inline static string tcp_addr;
    inline static string kafka_addr;
    inline static std::vector<int> esr_expected;

    inline static int CreateSettingsFrom_Cmd(int argc, char* argv[])
    {
        cout << "You have entered " << argc << " arguments:" << "\n";
        if (argc < 5) {
            cerr << "argv < 5" << endl;
            return -1;
        }
        for (int i {}; i < argc; ++i)
        {
            string arg=argv[i];
            if(arg == "--tcp_addr"){
                Settings::tcp_addr=argv[i+1];
            }
            if(arg == "--kafka_addr"){
                Settings::kafka_addr=argv[i+1];
            }
            if(arg == "--esr_expected") {
                string str;
                stringstream ss(argv[i+1]);
                while (getline(ss, str, ','))
                {
                    int intEsr=stoi(str);
                    Settings::esr_expected.emplace_back(intEsr);
                }
            }
            std::cout << arg << " ";
        }
        std::cout << endl << std::string (100, '-') << endl;
        return 0;
    }


    inline static int CreateSettingsFrom_Env()
    {
        env.load_dotenv(".env", true);
        Settings::tcp_addr= env["GID_URAL_TCP_ADDR"];
        Settings::kafka_addr= env["GID_URAL_KAFKA_ADDR"];
        string str;
        stringstream ss(env["GID_URAL_ESR_EXPECTED"]);
        while (getline(ss, str, ','))
        {
            int intEsr=stoi(str);
            Settings::esr_expected.emplace_back(intEsr);
        }
        cout << "GID_URAL_TCP_ADDR: " << env["GID_URAL_TCP_ADDR"] << endl;
        cout << "GID_URAL_KAFKA_ADDR: " << env["GID_URAL_KAFKA_ADDR"] << endl;
        cout << "GID_URAL_ESR_EXPECTED: " << env["GID_URAL_ESR_EXPECTED"] << endl;
        return 0;
    }


    inline static void ParseTcpAddress(string& ip, int& port)
    {
        string str;
        stringstream ss(tcp_addr);

        getline(ss, str, ':');
        ip=str;

        getline(ss, str, ':');
        int p=stoi(str);
        port=p;
    }

};

#endif //GIDURALCLIENT_SETTINGS_H
