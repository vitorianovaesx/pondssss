#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// Função utilitária para exibir uma mensagem de erro e encerrar o programa
void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Verifica se os argumentos foram fornecidos corretamente
    if (argc < 4) {
        fprintf(stderr,"[ERR] Uso: %s arquivo hostname porta\n", argv[0]);
        exit(0);
    }

    // Inicialização de variáveis
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Abre o descritor de arquivo no modo somente leitura
    char *file_name = argv[1];
    FILE *file;
    file = fopen(file_name, "r");

    // Converte a porta e o hostname em inteiros e struct "hostent"
    portno = atoi(argv[3]);
    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr,"[ERR] Host inexistente\n");
        exit(0);
    }

    // Cria um descritor de socket usando IPv4 e TCP (AF_INET, SOCK_STREAM, protocolo 0)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("[ERR] Falha ao abrir o socket");

    // Zera o buffer do endereço do servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Define o protocolo do endereço como IPv4
    serv_addr.sin_family = AF_INET;

    // Copia o endereço obtido para o endereço do servidor
    bcopy(
        (char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length
    );

    // Define o número da porta
    serv_addr.sin_port = htons(portno);

    // Tenta se conectar ao servidor
    if (
        connect(
            sockfd,
            (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)
        ) < 0
    ) 
        error("[ERR] Falha na conexão");

    // Cria o buffer onde o conteúdo do arquivo será armazenado
    char buffer[4096];
    bzero(buffer, 4096); // Zera o buffer por precaução

    // Itera sobre o arquivo, lendo em blocos de 4096 bytes e enviando um por um
    // Para o servidor até atingir o EOF (fim do arquivo)
    while(fgets(buffer, 4096, file)) {

        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("[ERR] Falha ao escrever no socket");

        // Limpa o buffer após o envio para que ele possa ser preenchido corretamente na próxima iteração
        bzero(buffer, 4096);
    }

    // Ao atingir EOF, o programa pode ser encerrado e o servidor lidará com o restante

    printf("[INFO] Arquivo enviado.\n");

    close(sockfd); // Fecha o socket e finaliza normalmente.

    printf("[INFO] Conexão encerrada.\n");

    return 0;
}
