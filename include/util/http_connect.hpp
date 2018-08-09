#ifndef http_connect_h__
#define http_connect_h__

#include "util/util.hpp"
#ifndef WIN32
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>    
#include "sstream"
#include "util/singleton.h"
#endif // !WIN32

class HttpConnect : public Singleton<HttpConnect>
{
    public:
        HttpConnect();
        ~HttpConnect();
        void SocketHttp(std::string host, std::string request);
        void PostData(std::string host, std::string path, std::string post_content);
        void GetData(std::string host, std::string path, std::string get_content);

};

#endif // http_connect_h__
