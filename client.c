#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>

int s, result ;
struct sockaddr_in peer;

int send_client(char buf[], size_t size_);
int recv_client();

int main(int argc, char * argv[]){
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        perror("Error calling socket");
        return 0;
    }
    peer.sin_family = AF_INET;
    peer.sin_port = htons(9006);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    result = connect(s, (struct sockaddr*)&peer, sizeof(peer));
    if(result){
        perror("Error calling connect");
        return 0;
    }
    char string[40];
    int res, i=0;
    char character;
    while(1){
        res = recv_client();
        if(res == 2){
            break;
        }
        memset(string, 0, sizeof(char)*40);
        i = 0;
        do{
            character = getchar();
            string[i]=character;
            fflush(stdin);
            i++;
        } while (character != '\n');
        string[i]='\0';
        res = send_client(string, sizeof(string));
    }
    printf("You guessed the number. Success. Completion of the program...\n");
    if(shutdown(s, 1) < 0){
        perror("Error calling shutdown");
        return 0;
    }
    return 0;
}

int send_client(char buf[], size_t size_){/*посылаем данные*/
    result = send(s, buf, size_, 0);
    if(result <= 0){
        perror("Error calling send");
        return 0;
    }
}

int recv_client(){/* читаем ответ сервера */
    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(s, &allreads);
    readmask = allreads;
    if(select(s + 1, &readmask, NULL, NULL, NULL) <= 0){
        perror("Error calling select");
        return 0;
    }
    if(FD_ISSET(s, &readmask)){
        char buffer[40];
        memset(buffer, 0, 40*sizeof(char));
        result = recv(s, buffer, sizeof(buffer) - 1, 0);
        if(result < 0){
            perror("Error calling recv");
            return 0;
        }
        if(result == 0){
            perror("Server disconnected");
            return 0;
        }
        printf("%s\n", buffer);
        if(strncmp(buffer, "Correct", 7) == 0){
            return 2;
        }
        return 0;
    }
    if(FD_ISSET(0, &readmask)){
        printf("No server response");
        return 0;
    }
}
