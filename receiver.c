#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[ERR] Uso: %s porta\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("[ERR] Erro ao tentar abrir o socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(portno); 

    if (
        bind(
            sockfd, 
            (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)
        ) < 0
    ) 
        error("[ERR] Erro ao fazer bind no endereço");

    if (listen(sockfd, 5) < 0) 
        error("[ERR] Erro ao escutar");

    printf("[INFO] Escutando clientes na porta %d\n", portno);
    printf("[INFO] Pressione Ctrl+C para parar o servidor\n");

    char buffer[4096];
    while (1) {
        
        clilen = sizeof(cli_addr);
        newsockfd = accept(
            sockfd, 
            (struct sockaddr *) &cli_addr, 
            &clilen
        );
        
        if (newsockfd < 0) 
            error("[ERR] Erro ao aceitar conexão");

        printf("[INFO] Conexão estabelecida.\n\n");

        while (1) {
            // Limpa o buffer
            bzero(buffer,4096);

            // Lê o máximo de bytes possível do socket
            n = read(newsockfd,buffer,4096);

            if (n < 0) error("[ERR] Erro ao ler do socket"); // Erro na leitura

            if (n == 0) break; // EOF (fim da conexão)

            // Imprime somente a quantidade de bytes lida (n) na saída padrão
            fwrite(buffer, sizeof(char), n, stdout);
        }

        printf("\n\n[INFO] Conexão encerrada.\n");

        close(newsockfd);
    }

    close(sockfd);

    return 0; 
}
