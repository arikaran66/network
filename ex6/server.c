#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 9090
#define BUFFER_SIZE 1024
#define CREDENTIALS_FILE "users.txt"

int isPalindrome(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
        len--;
    }
    int left = 0, right = len - 1;
    while (left < right) {
        if (tolower(str[left]) != tolower(str[right]))
            return 0;
        left++;
        right--;
    }
    return 1;
}

int authenticateUser(char *username, char *password) {
    FILE *fp = fopen(CREDENTIALS_FILE, "r");
    if (fp == NULL) {
        printf("Error opening users.txt\n");
        return 0;
    }
    char fileUser[BUFFER_SIZE], filePass[BUFFER_SIZE];
    while (fscanf(fp, "%s %s", fileUser, filePass) == 2) {
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char username[BUFFER_SIZE], password[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server started on port %d\nWaiting for client...\n\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected from %s:%d\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Ask username
        send(client_fd, "SEND_USERNAME", strlen("SEND_USERNAME"), 0);
        memset(username, 0, BUFFER_SIZE);
        recv(client_fd, username, BUFFER_SIZE - 1, 0);
        username[strcspn(username, "\n")] = '\0';
        printf("Username received: %s\n", username);

        usleep(100000);

        // Ask password
        send(client_fd, "SEND_PASSWORD", strlen("SEND_PASSWORD"), 0);
        memset(password, 0, BUFFER_SIZE);
        recv(client_fd, password, BUFFER_SIZE - 1, 0);
        password[strcspn(password, "\n")] = '\0';
        printf("Password received: %s\n", password);

        usleep(100000);

        if (authenticateUser(username, password)) {
            send(client_fd, "LOGIN_SUCCESS", strlen("LOGIN_SUCCESS"), 0);
            printf("Login successful for %s\n\n", username);
        } else {
            send(client_fd, "LOGIN_FAILED", strlen("LOGIN_FAILED"), 0);
            printf("Login failed for %s\n\n", username);
            close(client_fd);
            continue;
        }

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            memset(response, 0, BUFFER_SIZE);

            int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0)
                break;

            int choice = atoi(buffer);

            if (choice == 3) {
                printf("%s chose Exit\n", username);
                break;
            }

            if (choice == 1)
                printf("%s chose Echo\n", username);
            else if (choice == 2)
                printf("%s chose Palindrome Check\n", username);

            memset(buffer, 0, BUFFER_SIZE);
            bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0)
                break;

            buffer[strcspn(buffer, "\n")] = '\0';
            printf("Input received: %s\n", buffer);

            if (choice == 1) {
                snprintf(response, BUFFER_SIZE, "%s", buffer);
                printf("Sent back: %s\n\n", response);
            }
            else if (choice == 2) {
                if (isPalindrome(buffer))
                    snprintf(response, BUFFER_SIZE, "%s is a Palindrome", buffer);
                else
                    snprintf(response, BUFFER_SIZE, "%s is NOT a Palindrome", buffer);
                printf("Sent back: %s\n\n", response);
            }
            else {
                snprintf(response, BUFFER_SIZE, "Invalid choice");
                printf("Sent back: %s\n\n", response);
            }

            send(client_fd, response, strlen(response), 0);
        }

        printf("%s disconnected\n\nWaiting for client...\n\n", username);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
