#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <map>

#include "convutils/json2xml.hpp"

#define VERSION 23
#define BUFSIZE 8096
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404

#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif

struct
    {
     std::string ext;
     std::string filetype;
    } extensions [] =
    {
        {"json", "text/json" },
        {"xml",  "text/xml" },
        {"gif", "image/gif" },
        {"jpg", "image/jpg" },
        {"jpeg","image/jpeg"},
        {"png", "image/png" },
        {"ico", "image/ico" },
        {"zip", "image/zip" },
        {"gz",  "image/gz"  },
        {"tar", "image/tar" },
        {"htm", "text/html" },
        {"html","text/html" },
        {"",""}
    };


void logger(int type, const std::string &s1, const std::string &s2, int socket_fd)
{
    int fd ;
    char logbuffer[BUFSIZE*2];

    switch (type)
    {
        case ERROR:
            sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1.c_str(), s2.c_str(), errno,getpid());
            break;

        case FORBIDDEN:
            (void)write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\n"
                                   "Connection: close\nContent-Type: text/html\n\n<html><head>\n"
                                   "<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\n"
                                   "The requested URL, file type or operation is not allowed on this "
                                   "simple static file webserver.\n</body></html>\n", 271);
            sprintf(logbuffer,"FORBIDDEN: %s:%s",s1.c_str(), s2.c_str());;
            break;

        case NOTFOUND:
            (void)write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\n"
                                   "Content-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n"
                                   "</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this "
                                   "server.\n</body></html>\n", 224);
            sprintf(logbuffer,"NOT FOUND: %s:%s",s1.c_str(), s2.c_str());
            break;

        case LOG:
            sprintf(logbuffer," INFO: %s:%s:%d",s1.c_str(), s2.c_str(),socket_fd);
            break;

        default:
            break;
    }

//    std::cout << logbuffer<< std::endl << std::endl;

    /* No checks here, nothing can be done with a failure anyway */
    if((fd = open("nweb.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0)
    {
        write(fd, logbuffer, strlen(logbuffer));
        write(fd,"\n",1);
        close(fd);
    }

    if(type == ERROR || type == NOTFOUND || type == FORBIDDEN)
        exit(3);
}
 
/* this is a child web server process, so we can exit on errors */
void web(int fd, int hit)
{
    int j, file_fd, buflen;
    long i, ret, len;
    std::string fstr("");
    static char buffer[BUFSIZE+1];      /* static so zero filled */

    ret = read(fd,buffer,BUFSIZE);      /* read Web request in one go */

    if(ret == 0 || ret == -1)
    {
        /* read failure stop now */
        logger(FORBIDDEN,"failed to read browser request","", fd);
    }

    if(ret > 0 && ret < BUFSIZE)        /* return code is valid chars */
        buffer[ret]=0;                  /* terminate the buffer */
    else
        buffer[0]=0;

    for(i=0;i<ret;i++)                  /* remove CR and LF characters */
        if(buffer[i] == '\r' || buffer[i] == '\n')
            buffer[i]='*';

    logger(LOG,"request",buffer,hit);

    if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) )
    {
        logger(FORBIDDEN,"Only simple GET operation supported",buffer,fd);
    }

    for(i=4;i<BUFSIZE;i++)
    {   /* null terminate after the second space to ignore extra stuff */
        if(buffer[i] == ' ')
        {   /* string is "GET URL " +lots of other stuff */
            buffer[i] = 0;
            break;
        }
    }

    for(j=0;j<i-1;j++)   /* check for illegal parent directory use .. */
    {
        if(buffer[j] == '.' && buffer[j+1] == '.')
        {
            logger(FORBIDDEN, "Parent directory (..) path names not supported", buffer, fd);
        }
    }

    /* convert no filename to index file */
    if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) )
        (void)strcpy(buffer,"GET /index.html");

    /* work out the file type and check we support it */
    buflen = strlen(buffer);

    for(i=0; !extensions[i].ext.empty(); i++)
    {
        len = (extensions[i].ext.length());

        if( !strncmp(&buffer[buflen-len], extensions[i].ext.c_str(), len))
        {
            fstr = extensions[i].filetype;
            // check for JSON file request and convert it to XML
            if(!strncmp("json", extensions[i].ext.c_str(), len))
            {
                std::string json_file(&buffer[5]);
                std::ifstream istream(&buffer[5]);
                std::ostringstream oss;
                oss << istream.rdbuf();

                std::string xml_string = Json2Xml::json2xml(oss.str().data());

                // Save to file
                if(!xml_string.empty())
                {
                    size_t start_pos = json_file.find("json");

                    std::string xml_file(json_file.replace(start_pos, sizeof("xml "), "xml "));
                    std::ofstream file_stored(xml_file.c_str());
                    file_stored << xml_string;
                    file_stored.close();

                    fstr = extensions[1].filetype;
                    // where is to output ??
                    memcpy(&buffer[5], xml_file.data(), xml_file.length());
                }
            }
            break;
        }
    }

    if(fstr.empty())
        logger(FORBIDDEN,"file extension type not supported",buffer,fd);

    if(( file_fd = open(&buffer[5],O_RDONLY)) == -1)
    {  /* open the file for reading */
        logger(NOTFOUND, "failed to open file", &buffer[5], fd);
    }

    logger(LOG,"SEND",&buffer[5],hit);

    len = (long)lseek(file_fd, (off_t)0, SEEK_END); /* lseek to the file end to find the length */

    (void)lseek(file_fd, (off_t)0, SEEK_SET); /* lseek back to the file start ready for reading */
    (void)sprintf(buffer,"HTTP/1.1 200 OK\nServer: nweb/%d.0\nContent-Length: %ld\n"
                         "Connection: close\nContent-Type: %s\n\n", VERSION, len, fstr.c_str()); /* Header + a blank line */

    logger(LOG,"Header",buffer,hit);

    (void)write(fd,buffer,strlen(buffer));

    /* send file in 8KB block - last block may be smaller */
    while (  (ret = read(file_fd, buffer, BUFSIZE)) > 0 )
    {
        (void)write(fd,buffer,ret);
    }

    sleep(1);  /* allow socket to drain before signalling the socket is closed */

    close(fd);

    exit(1);
}

int main(int argc, char **argv)
{
    int i, port(8181), pid, listenfd, socketfd, hit;
    std::string rootDir("./");
    socklen_t length;
    static struct sockaddr_in cli_addr; /* static = initialised to zeros */
    static struct sockaddr_in serv_addr; /* static = initialised to zeros */
 
    if( argc > 1  )
    {
        if(!strcmp(argv[1], "-?"))
        {
            logger(LOG,"hint: nweb Port-Number Top-Directory\t\tversion %d\n\n"
                       "\tnweb is a small and very safe mini web server\n"
                       "\tnweb only servers out file/web pages with extensions named below\n"
                       "\t and only from the named directory or its sub-directories.\n"
                       "\tThere is no fancy features = safe and secure.\n\n"
                       "\tExample: nweb 8181 /home/nwebdir &\n\n"
                       "\tOnly Supports:", " ", 0);

            for(i=0; !extensions[i].ext.empty(); i++)
                (void)printf("    %s",extensions[i].ext.c_str());

            (void)printf( "\n\tNot Supported: URLs including \"..\", Java, Javascript, CGI\n"
                          "\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n"
                          "\tNo warranty given or implied\n\tNigel Griffiths nag@uk.ibm.com\n"  );
        }

    }
    else if(argc > 2 )
    {
        port = atoi(argv[1]);
        rootDir = argv[2];
    }

    const char *rootDirChr(rootDir.c_str());

    if( !strncmp(rootDirChr,"/"   ,2 ) || !strncmp(rootDirChr,"/etc", 5 ) ||
        !strncmp(rootDirChr,"/bin",5 ) || !strncmp(rootDirChr,"/lib", 5 ) ||
        !strncmp(rootDirChr,"/tmp",5 ) || !strncmp(rootDirChr,"/usr", 5 ) ||
        !strncmp(rootDirChr,"/dev",5 ) || !strncmp(rootDirChr,"/sbin",6) )
    {
        logger(LOG,"ERROR: Bad top directory %s, see nweb -?\n",rootDirChr,0);
        exit(3);
    }

    if(chdir(rootDir.c_str()) == -1)
    {
        logger(LOG,"ERROR: Can't Change to directory \n",rootDir.c_str(),0);
        exit(4);
    }

    /* Become deamon + unstopable and no zombies children (= no wait()) */
    if(fork() != 0)
        return 0; /* parent returns OK to shell */
    (void)signal(SIGCLD, SIG_IGN); /* ignore child death */
    (void)signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */
    for(i=0;i<32;i++)
        (void)close(i);		/* close open files */
    (void)setpgrp();		/* break away from process group */

    /* setup the network socket */
    if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
      logger(ERROR, "system call","socket",0);

    if(port < 0 || port >60000)
      logger(ERROR,"Invalid port number (try 1->60000)",argv[1],0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
        logger(ERROR,"system call","bind",0);

    if( listen(listenfd,64) <0)
        logger(ERROR,"system call","listen",0);

    for(hit=1; ;hit++)
    {
        length = sizeof(cli_addr);

        if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
            logger(ERROR,"system call","accept",0);

        if((pid = fork()) < 0)
        {
            logger(ERROR,"system call","fork",0);
        }
        else
        {
            if(pid == 0)
            {   /* child */
                (void)close(listenfd);
                web(socketfd,hit); /* never returns */
            }
            else
            {   /* parent */
                (void)close(socketfd);
            }
        }
    }
}
