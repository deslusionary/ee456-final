#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>

#define INCOMING_PORT 36000 
#define BACKLOG 10
using namespace std;


int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("socket()");
        return -1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(INCOMING_PORT); 
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind()");
        return -1;
    }

    // listen
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen()");
        return -1;
    }
    cout << "\033[34mServer listening for connection\033[39m" << endl;

    // loop for accepts -- can only handle one connection!
    for (;;) {
        int new_sockfd;
        struct sockaddr_in new_addr;
        int new_addrlen;

        if ((new_sockfd = accept(sockfd, (struct sockaddr *) &new_addr, (socklen_t *) &new_addrlen)) == -1) {
            perror("accept()");
            return -1;
        }
        cout << "\033[34mServer accepted connection\033[39m" << endl;

        char buf[1024];
        int len;
        int len_sent;
        len = recv(new_sockfd, buf, 1024, 0);
        printf("\033[34mReceived %d bytes from client\033[39m\n", len);

        buf[len] = '\n';
        printf("%s\n", buf);
        
        //len_sent = send(new_sockfd, buf, len, 0);
        //printf("Echoed %d bytes back to client\n", len_sent);
        close(new_sockfd);
    }
    
    close(sockfd);
    return 0;
}
