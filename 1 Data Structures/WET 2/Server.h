//
// Created by 97254 on 02/01/2020.
//

#ifndef MIVNIRATUV2_SERVER_H
#define MIVNIRATUV2_SERVER_H


class Server {
    int serverID;
    int dataCenterID;
    int traffic;

public:
    Server(int serverID, int dataCenterID, int traffic):
            serverID(serverID), dataCenterID(dataCenterID), traffic(traffic){}
    ~Server() = default;
    int getDataCenterID() const{
        return dataCenterID;
    }
    int getTraffic() const{
        return  traffic;
    }
    void changeTraffic(int traffic){
        this->traffic = traffic;
    }
    int getServerID() const{
        return  serverID;
    }
    friend bool operator>(Server s1, Server s2);
    friend bool operator<(Server s1, Server s2);

};

bool operator>(Server s1, Server s2){
    return (s1.traffic != s2.traffic ? s1.traffic > s2.traffic : s1.serverID < s2.serverID);
}

bool operator<(Server s1, Server s2){
    return (s1.traffic != s2.traffic ? s1.traffic < s2.traffic : s1.serverID > s2.serverID);
}
#endif //MIVNIRATUV2_SERVER_H