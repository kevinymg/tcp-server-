/*
 * connection_pool.cpp
 *
 *  Created on: 2013-3-29
 *      Author: csc
 */
 
#include <stdexcept>
#include <exception>
#include <stdio.h>
#include <mysql_driver.h>
#include "connection_pool.h"
 
using namespace std;
using namespace sql;
 
ConnPool *ConnPool::connPool = NULL;
 

ConnPool::ConnPool(string url, string userName, string password, int maxSize) {
    this->maxSize = maxSize;
    this->curSize = 0;
    this->username = userName;
    this->password = password;
    this->url = url;
    try {
        this->driver = sql::mysql::get_driver_instance();
    } catch (sql::SQLException&e) {
        perror("sql exception on get_driver_instance\n");
    } catch (std::runtime_error&e) {
        perror("run time error on get_driver_instance\n");
    }
    this->InitConnection(maxSize / 2);
}

ConnPool*ConnPool::GetInstance() {
    if (connPool == NULL) {
        connPool = new ConnPool("tcp://127.0.0.1:3306", "root", "ymg", 2000);
    }
    return connPool;
}
 

void ConnPool::InitConnection(int iInitialSize) {
    Connection*conn;
    pthread_mutex_lock(&lock);
    for (int i = 0; i < iInitialSize; i++) {
        conn = this->CreateConnection();
        if (conn) {
            connList.push_back(conn);
            ++(this->curSize);
        } else {
            perror("create connection error\n");
        }
    }
    pthread_mutex_unlock(&lock);
}
 

Connection* ConnPool::CreateConnection() {
    Connection*conn;
    try {
        conn = driver->connect(this->url, this->username, this->password); 
        return conn;
    } catch (sql::SQLException&e) {
        perror("connect exception error\n");
        return NULL;
    } catch (std::runtime_error&e) {
        perror("connect run time error\n");
        return NULL;
    }
}
 

Connection*ConnPool::GetConnection() {
    Connection*con;
    pthread_mutex_lock(&lock);
 
    if (connList.size() > 0) {   
        con = connList.front(); 
        connList.pop_front();   
        if (con->isClosed()) {   
            delete con;
            con = this->CreateConnection();
        }
        
        if (con == NULL) {
            --curSize;
        }
        pthread_mutex_unlock(&lock);
        return con;
    } else {
        if (curSize < maxSize) { 
            con = this->CreateConnection();
            if (con) {
                ++curSize;
                pthread_mutex_unlock(&lock);
                return con;
            } else {
                pthread_mutex_unlock(&lock);
                return NULL;
            }
        } else { 
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }
}
 

void ConnPool::ReleaseConnection(sql::Connection * conn) {
    if (conn) {
        pthread_mutex_lock(&lock);
        connList.push_back(conn);
        pthread_mutex_unlock(&lock);
    }
}
 

ConnPool::~ConnPool() {
    this->DestoryConnPool();
}
 

void ConnPool::DestoryConnPool() {
    list<Connection*>::iterator icon;
    pthread_mutex_lock(&lock);
    for (icon = connList.begin(); icon != connList.end(); ++icon) {
        this->DestoryConnection(*icon); 
    }
    curSize = 0;
    connList.clear(); 
    pthread_mutex_unlock(&lock);
}
 

void ConnPool::DestoryConnection(Connection* conn) {
    if (conn) {
        try {
            conn->close();
        } catch (sql::SQLException&e) {
            perror(e.what());
        } catch (std::exception&e) {
            perror(e.what());
        }
        delete conn;
    }
}
