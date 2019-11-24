#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#include <stdlib.h>
#include <time.h>
#include <stddef.h>

int s1;
struct sockaddr_in addr;
int secret, number;

int send_server();
void recv_server(char response[], size_t size_);

int main(int argc, char * argv){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        perror("Error calling socket");
        return 0;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9006);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);//ќн св€зывает сокет со всеми доступными интерфейсами.
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("Error calling bind");
        return 0;
    }
    if(listen(s, 5)){
        perror("Error calling listen");
        return 0;
    }
    s1 = accept(s, NULL, NULL);
    if(s1 < 0){
        perror("Error calling accept");
        return 0;
    }
    srand(time(NULL));
    int res;
    secret = rand() % 10 + 1;
    printf("Secret = %d \n", secret);
    char response[] = "I am thinking 1-10::<guess> <argument>";
    char response_more[] = "More";
    char response_less[] = "Less";
    char response_correct[] = "Correct";
    char response_err[] = "Incorrect format";
    recv_server(response, sizeof(response));
    while(1){
        res = send_server();
        if(res == -1){
            recv_server(response_err, sizeof(response_err));
        }
        if(number > secret)
            recv_server(response_less, sizeof(response_less));
        else if(number < secret)
            recv_server(response_more, sizeof(response_more));
        else if(number == secret)
            recv_server(response_correct, sizeof(response_correct));
        number = 0;
    }
    return 0;
}

int send_server(){/*читаем данные из сокета*/
    char buffer[40], result[40];
    int counter = 0, i, j = 0;
    memset(buffer, 0, sizeof(char)*40);
    memset(result, 0, sizeof(char)*40);
    int rc = recv(s1, buffer, 39, 0);
    printf("%s", buffer);
    if(rc < 0){
        if(errno == EINTR)
            return 0;
        perror("Can't receive data.");
        return 0;
    }
    if(rc == 0)
        return 0;
    char srt[]="<guess>";
    char *ptr = strstr(buffer, "<guess> <");
    if(ptr!= NULL){
    }else{
        return -1;
    }
    for(i = 0; i<40; i++){
        if(buffer[i]>='0' && buffer[i]<='9'){
            result[j]=buffer[i];
            j++;
        }
    }
    result[j] = '\0';
    number = atoi(result);
    return 0;
}

void recv_server(char response[], size_t size_){
    if(sendto( s1, response, size_, 0, (struct sockaddr *)&addr, sizeof(addr) ) < 0)
        perror("Error sending response");
}
