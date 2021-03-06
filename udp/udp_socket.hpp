/*********************************************************
 * 实现封装一个udp socket类
 * (1)bool Socket() 创建套接字
 * (2)bool Bind(std::string &ip ,uint16_t port) 
 * (3)bool Recv(std::string &buf,std::string &ip,uint16_t port)
 * (4)bool Send(std::string &buf,std::string &ip,uint16_t port)
 * (5)bool Close()
 * *******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class UdpSocket
{
    public:
        UdpSocket():_sockfd (-1)
    {}
        ~UdpSocket(){
            close(_sockfd);
        }

        bool Socket(){
        //int socket (int domain ,int type,int protocol)
        _sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd < 0){
            perror("socket error");
            return false;
        }
        return true; 
    }

    bool Bind(std::string &ip,uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        // 因为端口和地址信息在网络传输,需要字节序转换
        // 不可以用htonl 解析，因为范围过大造成
        addr.sin_port = htons(port); 
        //in_addr_t inet_addr(const char* cp)
        //将字符串点分十进制IP地址转换为网络字节序IP地址
        //sin_addr 也是一个结构体
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        //ip 是一个string类型，转换为c分格字符串
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = bind(_sockfd,(sockaddr*)&addr,len);
        if(ret < 0){
            perror("bind error");
            return false;
        }
        return true;
    }

    bool Recv(std::string &buf,std::string &ip,uint16_t &port){
        char tmp[4096] = {0};
        struct sockaddr_in addr;
        socklen_t len  = sizeof(struct sockaddr_in);
        int ret = recvfrom(_sockfd,tmp,4096,0,(sockaddr*)&addr,&len);
        if(ret < 0){
            perror("recvfrom error");
            return false;
        }

        //从tmp拷贝指定长度的数据到 buf 中
        buf.assign(tmp,ret);
        //从网络字节序转换为点分制字节ip
        ip = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        return true;
    }

    bool Send(std::string &buf,std::string &ip,uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = sendto(_sockfd,buf.c_str(),buf.size(),0,
                (struct sockaddr*)&addr,len);
        if(ret < 0){
            perror("sento error");
            return false;
        }
        return true;
    }

    bool Close(){
        close(_sockfd);
        return true;
    }
    private:
        int _sockfd;
};

