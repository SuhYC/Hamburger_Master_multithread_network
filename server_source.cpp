#include "main_server.h"

class Order {
public:
    std::string _bunOrder;
    std::string _pattyOrder;
    SOCKET _clnt_socket;

    Order(std::string bunOrder, std::string pattyOrder, SOCKET clnt_socket) {
        _bunOrder = bunOrder;
        _pattyOrder = pattyOrder;
        _clnt_socket = clnt_socket;
    }

    SOCKET GetSocket() {
        return _clnt_socket;
    }
};

class Ingredient {
protected:
    std::string state;
    clock_t cooking_start_time;
    clock_t cooking_end_time;

public:
    virtual ~Ingredient() {    }

    void StartCooking() {
        cooking_start_time = clock();
        return;
    }
    std::string GetState() { return state; }
};

class Patty : public Ingredient {
private:
    std::string type;
public:
    Patty(std::string str) {
        type = str;
    };
    ~Patty() {    };

    std::string GetPattyType() { return type; }
    void EndCooking() {
        cooking_end_time = clock();
        double cooking_time = (double)(cooking_end_time - cooking_start_time) / CLOCKS_PER_SEC;
        if (cooking_time < 0.15) {
            state = "Raw";
        }
        else if (cooking_time < 0.3) {
            state = "Good";
        }
        else {
            state = "Coal";
        }

        return;
    }
    bool CookedEnough() {
        clock_t t = clock();
        if ((double)(t - cooking_start_time) / CLOCKS_PER_SEC < 0.15) {
            return false;
        }
        return true;
    }
};

class Bun : public Ingredient {
private:
    std::string type;
public:
    Bun(std::string str) {
        type = str;
    }
    ~Bun() {    };


    std::string GetBunType() {
        return type;
    }
    void EndCooking() {
        cooking_end_time = clock();
        double cooking_time = (double)(cooking_end_time - cooking_start_time) / CLOCKS_PER_SEC;
        if (cooking_time < 0.1) {
            state = "Cold";
        }
        else if (cooking_time < 0.2) {
            state = "Good";
        }
        else {
            state = "Coal";
        }

        return;
    }
    bool CookedEnough() {
        clock_t t = clock();
        if ((double)(t - cooking_start_time) / CLOCKS_PER_SEC < 0.1) {
            return false;
        }
        return true;
    }
};

class Refrigerator { // critical region
private:
    std::stack<Bun*> butterBunStack;
    std::stack<Bun*> sesameBunStack;
    std::stack<Patty*> beefPattyStack;
    std::stack<Patty*> chickenPattyStack;
    std::recursive_mutex bbsmu;
    std::recursive_mutex sbsmu;
    std::recursive_mutex bpsmu;
    std::recursive_mutex cpsmu;
public:
    Refrigerator() {
        for (int i = 0; i < 5; i++) {
            PushButterBun();
            PushSesameBun();
            PushBeefPatty();
            PushChickenPatty();
        }
    }
    ~Refrigerator() {

    }


    void PushButterBun() {
        std::lock_guard<std::recursive_mutex> lockGuard(bbsmu);
        butterBunStack.push(new Bun("Butter"));

        return;
    }
    void PushSesameBun() {
        std::lock_guard<std::recursive_mutex> lockGuard(sbsmu);
        sesameBunStack.push(new Bun("Sesame"));

        return;
    }
    void PushBeefPatty() {
        std::lock_guard<std::recursive_mutex> lockGuard(bpsmu);
        beefPattyStack.push(new Patty("Beef"));

        return;
    }
    void PushChickenPatty() {
        std::lock_guard<std::recursive_mutex> lockGuard(cpsmu);
        chickenPattyStack.push(new Patty("Chicken"));

        return;
    }

    Bun* GetButterBun() {
        std::lock_guard<std::recursive_mutex> lockGuard(bbsmu);

        if (butterBunStack.empty()) {
            for (int i = 0; i < 5; i++) {
                PushButterBun();
            }
        }
        return butterBunStack.top();
    }
    Bun* GetSesameBun() {
        std::lock_guard<std::recursive_mutex> lockGuard(sbsmu);

        if (sesameBunStack.empty()) {
            for (int i = 0; i < 5; i++) {
                PushSesameBun();
            }
        }

        return sesameBunStack.top();
    }
    Patty* GetBeefPatty() {
        std::lock_guard<std::recursive_mutex> lockGuard(bpsmu);

        if (beefPattyStack.empty()) {
            for (int i = 0; i < 5; i++) {
                PushBeefPatty();
            }
        }

        return beefPattyStack.top();
    }
    Patty* GetChickenPatty() {
        std::lock_guard<std::recursive_mutex> lockGuard(cpsmu);

        if (chickenPattyStack.empty()) {
            for (int i = 0; i < 5; i++) {
                PushChickenPatty();
            }
        }

        return chickenPattyStack.top();
    }
};

class Grill { // critical region
private:
    std::queue<Bun*> bunGrill;
    std::queue<Patty*> pattyGrill;
    std::mutex bgqmu;
    std::mutex pgqmu;
public:
    void PushBun(Bun* b) {
        std::lock_guard<std::mutex> lockGuard(bgqmu);
        bunGrill.push(b);
        b->StartCooking();

        return;
    }
    void PushPatty(Patty* p) {
        std::lock_guard<std::mutex> lockGuard(pgqmu);
        pattyGrill.push(p);
        p->StartCooking();

        return;
    }
    Bun* GetBun(std::string type) {
        std::lock_guard<std::mutex> lockGuard(bgqmu);
        if (bunGrill.empty()) {
            cout << "bunPattyGrill is empty" << endl;
            return nullptr;
        }

        if (!strcmp(bunGrill.front()->GetBunType().c_str(), type.c_str()) && bunGrill.front()->CookedEnough()) { // same type && cooked enough
            Bun* out = bunGrill.front();
            bunGrill.pop();
            out->EndCooking();

            return out;
        }
        return nullptr;
    }
    Patty* GetPatty(std::string type) {
        std::lock_guard<std::mutex> lockGuard(pgqmu);
        if (pattyGrill.empty()) {
            cout << "chickenPattyGrill is empty" << endl;
            return nullptr;
        }

        if (!strcmp(pattyGrill.front()->GetPattyType().c_str(), type.c_str()) && pattyGrill.front()->CookedEnough()) { // same type && cooked enough
            Patty* out = pattyGrill.front();
            pattyGrill.pop();
            out->EndCooking();

            return out;
        }

        return nullptr;
    }
};

class Burger {
private:
    Bun* _bun;
    Patty* _patty;
    std::string _bunOrder;
    std::string _pattyOrder;
public:
    Burger(Order order) {
        _bunOrder = order._bunOrder;
        _pattyOrder = order._pattyOrder;
    }
    ~Burger() {

    }


    std::string GetBunOrder() { return _bunOrder; }
    std::string GetPattyOrder() { return _pattyOrder; }
    int BurgerCheck() { // return score of burger
        int score = 0;
        if (!strcmp(_bunOrder.c_str(), _bun->GetBunType().c_str())) {
            score++;
        }
        if (!strcmp(_bun->GetState().c_str(), "Good")) {
            score++;
        }
        if (!strcmp(_pattyOrder.c_str(), _patty->GetPattyType().c_str())) {
            score++;
        }
        if (!strcmp(_patty->GetState().c_str(), "Good")) {
            score++;
        }
        return score;
    }
    void CompleteBurger(Bun* bun, Patty* patty) {
        _bun = bun;
        _patty = patty;

        return;
    }
    std::string Wrapping() {
        return _bun->GetBunType() + " " + _bun->GetState() + " " + _patty->GetPattyType() + " " + _patty->GetState();
    }
};

class multipleArgument {
private:
    Order* _order;
    std::mutex* _takemu;
    Refrigerator* _refrg;
    Grill* _gr;

public:
    multipleArgument(std::queue<Order*> &oq, std::mutex &ordermu, std::mutex &takemu, Refrigerator &refrg, Grill &gr) {
        std::lock_guard<std::mutex> lockGuard(ordermu);

        if (oq.empty()) { // no order
            cout << "영업 종료!" << endl;
        }
        else {
            _order = oq.front();
            oq.pop();
        }

        _takemu = &takemu;
        _refrg = &refrg;
        _gr = &gr;
    }

    Order* GetOrder() {
        return _order;
    }
    std::mutex* GetTakeMutex() {
        return _takemu;
    }
    Refrigerator* GetRefrigerator() {
        return _refrg;
    }
    Grill* GetGrill() {
        return _gr;
    }
};

void SendMsg(SOCKET socket, std::string msg, int len, std::mutex& mu) {
    std::lock_guard<std::mutex> lockGuard(mu);
    send(socket, msg.c_str(), len, 0);
    cout << msg + "배달 완료!" << endl;
    return;
}

void MakeBurger(multipleArgument &mArg) {

    Refrigerator* refrigerator = mArg.GetRefrigerator();
    Grill* grill = mArg.GetGrill();

    Order* order = mArg.GetOrder();

    Burger* b = new Burger(*order);

    Bun* bun = nullptr;
    Patty* patty = nullptr;

    if (!strcmp(order->_bunOrder.c_str(), "Butter")) { // take bun
        bun = refrigerator->GetButterBun();
    }
    else {
        bun = refrigerator->GetSesameBun();
    }

    if (!strcmp(order->_pattyOrder.c_str(), "Beef")) { // take patty
        patty = refrigerator->GetBeefPatty();
    }
    else {
        patty = refrigerator->GetChickenPatty();
    }

    grill->PushBun(bun);
    bun = nullptr;
    grill->PushPatty(patty);
    patty = nullptr;

    while (bun == nullptr || patty == nullptr) {
        if (bun == nullptr) {
            bun = grill->GetBun(order->_bunOrder);
        }
        if (patty == nullptr) {
            patty = grill->GetPatty(order->_pattyOrder);
        }
    }

    b->CompleteBurger(bun, patty);
    int score = b->BurgerCheck();

    std::mutex* takemu = mArg.GetTakeMutex();

    SOCKET s = order->GetSocket();

    std::string str = bun->GetBunType() + patty->GetPattyType() + std::to_string(score) + "/";

    SendMsg(s, str, (int)strlen(str.c_str()), *takemu);

    return;
}


int main(void)
{
    
    WORD		wVersionRequested;
    WSADATA		wsaData;
    SOCKADDR_IN servAddr, cliAddr; //Socket address information
    int			err;
    char        buf[BUF_SIZE]; // recv buf

    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {
        cout << "WSAStartup error " << WSAGetLastError() << endl;
        WSACleanup();
        return false;
    }

    servAddr.sin_family = AF_INET; // address family Internet
    servAddr.sin_port = htons(SERVER_PORT); //Port to connect on
    inet_pton(AF_INET, IPAddress, &(servAddr.sin_addr.s_addr)); //servAddr.sin_addr.s_addr = inet_addr(IPAddress); //Target IP


    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
    if (s == INVALID_SOCKET)
    {
        cout << "Socket error " << WSAGetLastError() << endl;
        WSACleanup();
        return false; //Couldn't create the socket
    }

    int x = bind(s, reinterpret_cast<SOCKADDR*>(&servAddr), sizeof(servAddr));
    if (x == SOCKET_ERROR)
    {
        cout << "Binding failed. Error code: " << WSAGetLastError() << endl;
        WSACleanup();
        return false; //Couldn't connect
    }

    std::queue<Order*> orderQueue;
    std::mutex ordermu;
    std::mutex takemu;
    Refrigerator refrigerator;
    Grill grill;


    std::vector<std::thread> workers;
    workers.resize(45);



    cout << "Waiting for client..." << endl;

    listen(s, 5);
    int xx = sizeof(cliAddr);

    while (true) {
        SOCKET s2 = accept(s, reinterpret_cast<SOCKADDR*>(&cliAddr), &xx);
        cout << "Connection established. New socket num is " << s2 << endl;


        int n = 0;
        n = recv(s2, buf, 400, 0);

        buf[n] = 0; // to make string, add null

        std::string str(buf); // get str from buffer

        int curposition = 0;
        int position = (int)str.find("/", curposition);

        int butter_beef_od = std::stoi(str.substr(curposition, position - curposition));

        curposition = position + 1;
        position = (int)str.find("/", curposition);

        int butter_chicken_od = std::stoi(str.substr(curposition, position - curposition));

        curposition = position + 1;
        position = (int)str.find("/", curposition);

        int sesame_beef_od = std::stoi(str.substr(curposition, position - curposition));

        curposition = position + 1;

        int sesame_chicken_od = std::stoi(str.substr(curposition));

        for (int i = 0; i < butter_beef_od; i++) { // relay order to kitchen
            orderQueue.push(new Order("Butter", "Beef", s2));
            cout << "BB" << endl;
        }
        for (int i = 0; i < butter_chicken_od; i++) {
            orderQueue.push(new Order("Butter", "Chicken", s2));
            cout << "BC" << endl;
        }
        for (int i = 0; i < sesame_beef_od; i++) {
            orderQueue.push(new Order("Sesame", "Beef", s2));
            cout << "SB" << endl;
        }
        for (int i = 0; i < sesame_chicken_od; i++) {
            orderQueue.push(new Order("Sesame", "Chicken", s2));
            cout << "SC" << endl;
        }

        cout << "주문 완료!" << endl;

        for (int i = 0; i < 12; i++) {
            multipleArgument* mArg = new multipleArgument(orderQueue, ordermu, takemu, refrigerator, grill);
            workers[i] = std::thread(MakeBurger, std::ref(*mArg));
        }

        for (int i = 0; i < 12; i++) {
            workers[i].join();
        }

    }


    closesocket(s);
    WSACleanup();

    return 0;
}


