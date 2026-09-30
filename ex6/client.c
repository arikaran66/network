#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    char choice_str[10];
    char username[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Wait for server to ask username
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (strcmp(buffer, "SEND_USERNAME") == 0) {
        printf("Enter username: ");
        fgets(username, BUFFER_SIZE, stdin);
        username[strcspn(username, "\n")] = '\0';
        send(sock_fd, username, strlen(username), 0);
    }

    // Wait for server to ask password
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (strcmp(buffer, "SEND_PASSWORD") == 0) {
        printf("Enter password: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';
        send(sock_fd, input, strlen(input), 0);
    }

    // Wait for login result
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (strcmp(buffer, "LOGIN_SUCCESS") == 0) {
        printf("Login successful! Welcome %s\n\n", username);
    } else {
        printf("Login failed! Invalid username or password.\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("1. Echo\n");
        printf("2. Palindrome Check\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        int choice;
        scanf("%d", &choice);
        getchar();

        snprintf(choice_str, sizeof(choice_str), "%d", choice);
        send(sock_fd, choice_str, strlen(choice_str), 0);
        usleep(100000);

        if (choice == 3) {
            printf("Goodbye %s!\n", username);
            break;
        }

        if (choice < 1 || choice > 3) {
            printf("Invalid choice. Try again.\n\n");
            continue;
        }

        printf("Enter a string or number: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        send(sock_fd, input, strlen(input), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server Response: %s\n\n", buffer);
    }

    close(sock_fd);
    return 0;
}
