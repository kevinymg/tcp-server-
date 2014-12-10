/*
 * main.cpp
 *
 *  Created on: 2013-3-26
 *      Author: holy
 */
 
#include "connection_pool.h"
 
namespace ConnectMySQL {
 
ConnPool *connpool = ConnPool::GetInstance();
 
void run() {
 
    Connection *con;
    Statement *state;
    ResultSet *result;
 
    con = connpool->GetConnection();
 
    state = con->createStatement();
    state->execute("use holy");
 
    result = state->executeQuery("select * from student");
 
    while (result->next()) {
        int id = result->getInt("number");
        string name = result->getString("sname");
        cout << id << " : " << name << endl;
    }
    delete state;
    connpool->ReleaseConnection(con);
}
}
 
int main(int argc, char* argv[]) {
    ConnectMySQL::run();
    return 0;
}

