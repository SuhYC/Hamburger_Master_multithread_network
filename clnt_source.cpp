#include "clnt_main.h"

int main(void)
{
    const int Butter_Beef = 3;
    const int Butter_Chicken = 3;
    const int Sesame_Beef = 3;
    const int Sesame_Chicken = 3;

    WORD		wVersionRequested;
    WSADATA		wsaData;
    SOCKADDR_IN target; //Socket address information
    SOCKET      s;
    int			err;
    char        buf[BUF_SIZE];

    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {
        printf("WSAStartup error %ld", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    target.sin_family = AF_INET; // address family Internet
    target.sin_port = htons(SERVER_PORT); //Port to connect on
    inet_pton(AF_INET, IPAddress, &(target.sin_addr.s_addr)); //target.sin_addr.s_addr = inet_addr(IPAddress)


    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
    if (s == INVALID_SOCKET)
    {
        cout << "socket() error : " << WSAGetLastError() << endl;
        WSACleanup();
        return 0; //Couldn't create the socket
    }


    if (connect(s, reinterpret_cast<SOCKADDR*>(&target), sizeof(target)) == SOCKET_ERROR)
    {
        cout << "connect() error : " << WSAGetLastError() << endl;
        cout << "서버 먼저 실행해주세요." << endl;
        WSACleanup();
        return 0; //Couldn't connect
    }



    printf("Sending Order...\n");

    std::string order_string = std::to_string(Butter_Beef) + "/" + std::to_string(Butter_Chicken) + "/" + std::to_string(Sesame_Beef) + "/" + std::to_string(Sesame_Chicken);

    if (strlen(order_string.c_str()) >= 400) {
        cout << "Too Much." << endl;
    }

    send(s, order_string.c_str(), (int)strlen(order_string.c_str()), 0); // use "send" in windows

    int bbcnt = 0, sbcnt = 0, bccnt = 0, sccnt = 0;
    int score_bb = 0, score_sb = 0, score_bc = 0, score_sc = 0;

    while (true) {
        int n = recv(s, buf, 400, 0); // read max 400 bytes
        if (n <= 0) { cout << "error!"; }
        buf[n] = 0; // make a string
        
        std::string str(buf);

        int curposition = 0;
        int position;
        while ((position = (int)str.find("/", curposition)) != std::string::npos) {
            int len = position - curposition;
            std::string res = str.substr(curposition, len);
            if (res[0] == 'B') {
                if (res[6] == 'B') {
                    bbcnt++;
                    score_bb += res[10] - '0';
                }
                else {
                    bccnt++;
                    score_bc += res[13] - '0';
                }
            }
            else if (res[0] == 'S'){
                if (res[6] == 'B') {
                    sbcnt++;
                    score_sb += res[10] - '0';
                }
                else {
                    sccnt++;
                    score_sc += res[13] - '0';
                }
            }
            else {
                break;
            }

            curposition = position + 1;
        }

        if (bbcnt != 0) {
            cout << "ButterBeef : " << bbcnt << " and Rating : " << ((double)score_bb / bbcnt) << endl;
        }
        if (bccnt != 0) {
            cout << "ButterChicken : " << bccnt << " and Rating : " << ((double)score_bc / bccnt) << endl;
        }
        if (sbcnt != 0) {
            cout << "SesameBeef : " << sbcnt << " and Rating : " << ((double)score_sb / sbcnt) << endl;
        }
        if (sccnt != 0) {
            cout << "SesameChicken : " << sccnt << " and Rating : " << ((double)score_sc / sccnt) << endl;
        }

        cout << endl;

        if (Butter_Beef == bbcnt && Butter_Chicken == bccnt && Sesame_Beef == sbcnt && Sesame_Chicken == sccnt) {
            break;
        }
    }

    closesocket(s);
    WSACleanup();

    return 0;
}