//
// Created by 97254 on 03/12/2019.
//

#ifndef MIVNI2_SERVER_H
#define MIVNI2_SERVER_H
#include <iostream>


class server {
private:
    int serverID;
    int os; // 0 for linux and 1 for windows
    bool isFree;
    friend class dataCenter;
public:
    server(): serverID(-1), os(0), isFree(true){};



};


#endif //MIVNI2_SERVER_H