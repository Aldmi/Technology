#ifndef GIDURALCLIENT_SETTINGS_H
#define GIDURALCLIENT_SETTINGS_H

#include <vector>
#include <iostream>

using namespace std;

struct Settings {
    inline static string tcp_addr;
    inline static string kafka_addr;
    inline static string kafka_topic;
    inline static std::vector<int> esr_expected;

    inline static int CreateSettingsFrom_Cmd(int argc, char* argv[])
    {
        cout << "You have entered " << argc << " arguments:" << "\n";
        if (argc < 7) {
            cerr << "argv < 7 " << endl;
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
            if(arg == "--kafka_topic"){
                Settings::kafka_topic=argv[i+1];
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
