#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>

#define PORT 8080
#define BUFFER_SIZE 2048

/* ---------- Book Structure ---------- */

typedef struct {
    char title[100];
    int available;
} Book;

Book library[5] = {
    {"C Programming", 1},
    {"Data Structures", 1},
    {"Computer Networks", 0},
    {"Operating Systems", 1},
    {"Database Systems", 1}
};

/* ---------- Arithmetic Function ---------- */

void arithmetic_server(int sockfd,
                       char *buffer,
                       struct sockaddr_in *cliaddr,
                       socklen_t len)
{
    int num1, num2, result;
    char op;

    if (sscanf(buffer, "%d %c %d",
               &num1, &op, &num2) == 3)
    {
        switch (op)
        {
            case '+':
                result = num1 + num2;
                sprintf(buffer, "Result = %d", result);
                break;

            case '-':
                result = num1 - num2;
                sprintf(buffer, "Result = %d", result);
                break;

            case '*':
                result = num1 * num2;
                sprintf(buffer, "Result = %d", result);
                break;

            case '/':
                if (num2 == 0)
                    strcpy(buffer, "Error: Division by zero");
                else
                {
                    result = num1 / num2;
                    sprintf(buffer, "Result = %d", result);
                }
                break;

            case '%':
                if (num2 == 0)
                    strcpy(buffer, "Error: Modulo by zero");
                else
                {
                    result = num1 % num2;
                    sprintf(buffer, "Result = %d", result);
                }
                break;

            default:
                strcpy(buffer, "Error: Invalid Operator");
        }
    }
    else
    {
        strcpy(buffer, "Error: Invalid Format");
    }

    sendto(sockfd,
           buffer,
           strlen(buffer) + 1,
           0,
           (struct sockaddr *)cliaddr,
           len);
}

/* ---------- Library Server Function ---------- */

void library_server(int sockfd,
                    struct sockaddr_in *cliaddr,
                    socklen_t len)
{
    int choice;
    char buffer[BUFFER_SIZE];

    recvfrom(sockfd,
             &choice,
             sizeof(choice),
             0,
             (struct sockaddr *)cliaddr,
             &len);

    memset(buffer, 0, BUFFER_SIZE);

    /* Search */
    if (choice == 1)
    {
        char query[100];

        recvfrom(sockfd,
                 query,
                 sizeof(query),
                 0,
                 (struct sockaddr *)cliaddr,
                 &len);

        int found = 0;

        for (int i = 0; i < 5; i++)
        {
            if (strcasecmp(library[i].title, query) == 0)
            {
                found = 1;

                if (library[i].available)
                    strcpy(buffer, "Book is available.");
                else
                    strcpy(buffer,
                           "Book is currently borrowed.");

                break;
            }
        }

        if (!found)
            strcpy(buffer,
                   "Book not found in library.");

        sendto(sockfd,
               buffer,
               strlen(buffer) + 1,
               0,
               (struct sockaddr *)cliaddr,
               len);
    }

    /* Return */
    else if (choice == 2)
    {
        char query[100];

        recvfrom(sockfd,
                 query,
                 sizeof(query),
                 0,
                 (struct sockaddr *)cliaddr,
                 &len);

        int found = 0;

        for (int i = 0; i < 5; i++)
        {
            if (strcasecmp(library[i].title, query) == 0)
            {
                found = 1;
                library[i].available = 1;

                strcpy(buffer,
                       "Book returned successfully.");

                break;
            }
        }

        if (!found)
            strcpy(buffer,
                   "Book does not belong to this library.");

        sendto(sockfd,
               buffer,
               strlen(buffer) + 1,
               0,
               (struct sockaddr *)cliaddr,
               len);
    }

    /* List Available Books */
    else if (choice == 3)
    {
        strcpy(buffer, "Available Books:\n");

        for (int i = 0; i < 5; i++)
        {
            if (library[i].available)
            {
                strcat(buffer, "- ");
                strcat(buffer, library[i].title);
                strcat(buffer, "\n");
            }
        }

        sendto(sockfd,
               buffer,
               strlen(buffer) + 1,
               0,
               (struct sockaddr *)cliaddr,
               len);
    }
}

/* =========================================================
                         SERVER
   ========================================================= */

void run_server()
{
    int sockfd;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    /* Create UDP socket */
    if ((sockfd = socket(AF_INET,
                         SOCK_DGRAM,
                         0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    /* Bind */
    if (bind(sockfd,
             (struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("\n====================================\n");
    printf(" UDP SERVER\n");
    printf("====================================\n");
    printf("Server running on port %d...\n", PORT);

    while (1)
    {
        int program;

        memset(buffer, 0, BUFFER_SIZE);

        /*
         * First message tells the server
         * which program is being requested.
         */
        recvfrom(sockfd,
                 buffer,
                 BUFFER_SIZE,
                 0,
                 (struct sockaddr *)&cliaddr,
                 &len);

        program = atoi(buffer);

        /* Arithmetic */
        if (program == 1)
        {
            recvfrom(sockfd,
                     buffer,
                     BUFFER_SIZE,
                     0,
                     (struct sockaddr *)&cliaddr,
                     &len);

            printf("\nArithmetic request: %s\n",
                   buffer);

            arithmetic_server(sockfd,
                              buffer,
                              &cliaddr,
                              len);

            printf("Arithmetic result sent.\n");
        }

        /* Library */
        else if (program == 2)
        {
            printf("\nLibrary request received.\n");

            library_server(sockfd,
                           &cliaddr,
                           len);
        }

        /* Exit */
        else if (program == 3)
        {
            printf("\nClient requested exit.\n");
            break;
        }
    }

    close(sockfd);

    printf("UDP Server closed.\n");
}

/* =========================================================
                         CLIENT
   ========================================================= */

void run_client()
{
    int sockfd;

    char buffer[BUFFER_SIZE];
    char program[10];

    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    if ((sockfd = socket(AF_INET,
                         SOCK_DGRAM,
                         0)) < 0)
    {
        perror("Socket creation failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    /*
     * Localhost server
     */
    servaddr.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    while (1)
    {
        int choice;

        printf("\n====================================\n");
        printf(" UDP CLIENT MENU\n");
        printf("====================================\n");
        printf("1. Arithmetic Operations\n");
        printf("2. Library Management\n");
        printf("3. Exit\n");
        printf("====================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        sprintf(program, "%d", choice);

        sendto(sockfd,
               program,
               strlen(program) + 1,
               0,
               (struct sockaddr *)&servaddr,
               sizeof(servaddr));

        /* Arithmetic */
        if (choice == 1)
        {
            char expr[BUFFER_SIZE];

            printf("\nEnter expression ");
            printf("(example: 10 + 5): ");

            fgets(expr,
                  BUFFER_SIZE,
                  stdin);

            expr[strcspn(expr, "\n")] = '\0';

            sendto(sockfd,
                   expr,
                   strlen(expr) + 1,
                   0,
                   (struct sockaddr *)&servaddr,
                   sizeof(servaddr));

            memset(buffer, 0, BUFFER_SIZE);

            recvfrom(sockfd,
                     buffer,
                     BUFFER_SIZE,
                     0,
                     (struct sockaddr *)&servaddr,
                     &len);

            printf("Server Reply: %s\n",
                   buffer);
        }

        /* Library */
        else if (choice == 2)
        {
            int library_choice;

            printf("\n------ Library Management ------\n");
            printf("1. Search for a book\n");
            printf("2. Return a book\n");
            printf("3. List available books\n");
            printf("4. Back to main menu\n");

            printf("Enter choice: ");
            scanf("%d", &library_choice);
            getchar();

            if (library_choice == 4)
                continue;

            sendto(sockfd,
                   &library_choice,
                   sizeof(library_choice),
                   0,
                   (struct sockaddr *)&servaddr,
                   sizeof(servaddr));

            /* Search */
            if (library_choice == 1)
            {
                char book[100];

                printf("Enter book name: ");

                fgets(book,
                      sizeof(book),
                      stdin);

                book[strcspn(book, "\n")] = '\0';

                sendto(sockfd,
                       book,
                       strlen(book) + 1,
                       0,
                       (struct sockaddr *)&servaddr,
                       sizeof(servaddr));

                memset(buffer, 0, BUFFER_SIZE);

                recvfrom(sockfd,
                         buffer,
                         BUFFER_SIZE,
                         0,
                         (struct sockaddr *)&servaddr,
                         &len);

                printf("Response: %s\n",
                       buffer);
            }

            /* Return */
            else if (library_choice == 2)
            {
                char book[100];

                printf("Enter book name to return: ");

                fgets(book,
                      sizeof(book),
                      stdin);

                book[strcspn(book, "\n")] = '\0';

                sendto(sockfd,
                       book,
                       strlen(book) + 1,
                       0,
                       (struct sockaddr *)&servaddr,
                       sizeof(servaddr));

                memset(buffer, 0, BUFFER_SIZE);

                recvfrom(sockfd,
                         buffer,
                         BUFFER_SIZE,
                         0,
                         (struct sockaddr *)&servaddr,
                         &len);

                printf("Response: %s\n",
                       buffer);
            }

            /* List */
            else if (library_choice == 3)
            {
                memset(buffer, 0, BUFFER_SIZE);

                recvfrom(sockfd,
                         buffer,
                         BUFFER_SIZE,
                         0,
                         (struct sockaddr *)&servaddr,
                         &len);

                printf("\n%s\n", buffer);
            }
        }

        /* Exit */
        else if (choice == 3)
        {
            printf("\nExiting client...\n");
            break;
        }

        else
        {
            printf("\nInvalid choice!\n");
        }
    }

    close(sockfd);
}

/* =========================================================
                          MAIN
   ========================================================= */

int main()
{
    int choice;

    while (1)
    {
        printf("\n\n====================================\n");
        printf(" UDP NETWORK PROGRAM\n");
        printf("====================================\n");
        printf("1. Run as UDP Server\n");
        printf("2. Run as UDP Client\n");
        printf("3. Exit\n");
        printf("====================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            run_server();
        }
        else if (choice == 2)
        {
            run_client();
        }
        else if (choice == 3)
        {
            printf("Program terminated.\n");
            break;
        }
        else
        {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
