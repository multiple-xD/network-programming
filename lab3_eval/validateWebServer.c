#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void isDomain(char *host);
void checkFor(char *service, char *host);

void main (void)
{
    // Reading from webpage.txt
    FILE *fp;
    char host[100];
    char* filename = "./webpages.txt";

    fp = fopen(filename, "r");
    while (fgets(host, 100, fp) != NULL){
        host[strlen(host)-1] = '\0';
        isDomain(host);
    }
    fclose(fp);
}

void isDomain(char *host){

    struct addrinfo hints, *res;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // If getaddrinfo returns that means the domain exists
    if(getaddrinfo(host, NULL, &hints, &res))
        printf("**************************************************\nDomain %s does NOT exist\n", host);
    else
        printf("**************************************************\nDomain %s exists\n", host);

    // Checking for each service by passing the service argument in getaddrinfo
    checkFor("http", host);
    checkFor("ftp", host);
    /**********************************************
    ONLY THESE TWO SERVICES ARE GETTING A RESPONSE 

    checkFor("ntp", host);
    checkFor("telnet", host);
    checkFor("ftp", host);
    checkFor("dns", host);
    checkFor("ssh", host);
    checkFor("telnet", host);
    checkFor("smtp", host);
    checkFor("ntp", host);
    ***********************************************/
}

void checkFor(char *service, char *host){
    struct addrinfo hints, *res;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // Passing the service argument to getaddrinfo which returns the port no in the ai_addr socket structure
    if(getaddrinfo(host, service, &hints, &res)){
        printf("%s service not running as domain does not exist\n", service);
        return;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if(connect(sock, res->ai_addr, res->ai_addrlen))
        printf("%s service is not running on the webserver\n", service);
    else{
        struct sockaddr_in *address = (struct sockaddr_in *)res -> ai_addr;
        printf("%s service is running on port %d\n", service, ntohs(address -> sin_port));
    }
}
