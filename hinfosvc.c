/*
* IPK Projekt 1
* Main file
* Author: Ivan Tsiareshkin (xtsiar00)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CONST_HEADER_LENGTH 46 // Length of response http header

char response[256] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";

int Socket(int domain, int type, int protocol) {
    int response = socket(domain, type, protocol);
    if (response == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    return response;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int response = bind(sockfd, addr, addrlen);
    if (response == -1) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog) {
    int response = listen(sockfd, backlog);
    if (response == -1) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int response = accept(sockfd, addr, addrlen);
    if (response == -1) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    return response;
}

/*
* @brief Function of obtaining hostname
*
* @return Returns a string with the hostname and HTTP header at the beginning
*/
char* host_name() {
    char bashcmd[] = "cat /proc/sys/kernel/hostname";

    FILE *f = popen (bashcmd, "r");
    char s[128];

    while (fgets (s, sizeof (s), f)) strcat(response, s);

    pclose (f);
    return response;
}

/*
* @brief Function of obtaining CPU model name.
*
* @return Returns a string with the CPU name and HTTP header at the beginning
*/
char* cpu_name() {
    char bashcmd[] = "cat /proc/cpuinfo | grep 'model name' | cut -f 2 -d ':' | head -n 1 | awk '{$1 = $1}1'";

    FILE *f = popen (bashcmd, "r");
    char s[128];

    while (fgets (s, sizeof (s), f)) strcat(response, s);

    pclose (f);
    return response;
}

/*
* @brief Function of obtaining CPU load.
*
* @return Returns a string with load percentage and HTTP header at the beginning
*/
char* load() {
    char bashcmd[] = "cat /proc/stat | sed -n 1p | cut -c 5-";

    FILE *f = popen(bashcmd, "r");
    char s[512];
    char *end;
    long long int var[9];
    int i = 0;

    while(fgets(s, sizeof(s), f))

    for (int k = 0; s[k] != '\0'; k++) {
        if(s[k] == 32) {
            var[i] = strtol(&s[k], &end, 10);
            i++;
        }
    }

    long long int prevuser = var[0];
    long long int prevnice = var[1];
    long long int prevsystem = var[2];
    long long int previdle = var[3];
    long long int previowait = var[4];
    long long int previrq = var[5];
    long long int prevsoftirq = var[6];
    long long int prevsteal = var[7];

    sleep(1);

    FILE *f1 = popen(bashcmd, "r");
    char s1[512];
    char *end1;
    long long int var1[9];
    i = 0;

    while(fgets(s1, sizeof(s1), f1))

    for (int k = 0; s1[k] != '\0'; k++) {
        if(s1[k] == 32) {
            var1[i] = strtol(&s1[k], &end1, 10);
            i++;
        }
    }

    long long int user = var1[0];
    long long int nice = var1[1];
    long long int system = var1[2];
    long long int idle = var1[3];
    long long int iowait = var1[4];
    long long int irq = var1[5];
    long long int softirq = var1[6];
    long long int steal = var1[7];

    long long int PrevIdle = previdle + previowait;
    long long int Idle = idle + iowait;
    long long int PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    long long int NonIdle = user + nice + system + irq + softirq + steal;
    long long int PrevTotal = PrevIdle + PrevNonIdle;
    long long int Total = Idle + NonIdle;
    long long int totald = Total - PrevTotal;
    long long int idled = Idle - PrevIdle;
    long long int CPU_Percentage = (totald - idled);
    CPU_Percentage = 100 * CPU_Percentage / totald;

    char load[126];
    sprintf(load, "%lld", CPU_Percentage);
    strcat(response, load);
    strcat(response, "%\n");
    pclose(f);
    pclose(f1);
    return response;
}

int main(int argc, char const *argv[]) {
    int server = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in adr = {0};
    socklen_t adrlen = sizeof adr;

    int optval = 1;
    if ((setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(int))) == -1) {
        close(server);
        exit(EXIT_FAILURE);
    }

    adr.sin_family = AF_INET;
    adr.sin_port = htons(atoi(argv[1]));
    adr.sin_addr.s_addr = INADDR_ANY;

    Bind(server, (struct sockaddr*) &adr, sizeof adr);

    Listen(server, 5);

    while (1) {
        int fd = Accept(server, (struct sockaddr*) &adr, &adrlen);
        ssize_t nread;
        char buf[512];
        nread = read(fd, buf, 512);
        if (nread == -1) {
            exit(EXIT_FAILURE);
        }

        if (!strncmp(buf, "GET /hostname ", strlen("GET /hostname "))) {
            char *out = host_name();
            write(fd, out, strlen(out));
            response[CONST_HEADER_LENGTH] = '\0';

        } else if (!strncmp(buf, "GET /cpu-name ", strlen("GET /cpu-name "))) {
            char *out = cpu_name();
            write(fd, out, strlen(out));
            response[CONST_HEADER_LENGTH] = '\0';

        } else if (!strncmp(buf, "GET /load ", strlen("GET /load "))) {
            char *out = load();
            write(fd, out, strlen(out));
            response[CONST_HEADER_LENGTH] = '\0';
        } else {
            write(fd, "HTTP/1.1 400 Not Found\r\nContent-Type: text/plain;\r\n\r\n400 Not Found\n", strlen("HTTP/1.1 400 Not Found\r\nContent-Type: text/plain;\r\n\r\n400 Not Found\n"));
        }

        close(fd);
    }
    close(server);

    return 0;
}
