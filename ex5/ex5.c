#include <stdio.h>

void stopAndWait(int n, int lost)
{
    int i = 1;
    int transmissions = 0;

    printf("\n===== STOP-AND-WAIT ARQ =====\n");

    while (i <= n)
    {
        printf("\nSending frame %d...\n", i);
        transmissions++;

        if (i == lost)
        {
            printf("Frame %d lost/corrupted.\n", lost);
            printf("Timeout waiting for ACK %d.\n", lost);
            printf("Retransmitting frame %d...\n", lost);
            transmissions++;
            printf("ACK %d received.\n", lost);
            lost = 0; // Clear the loss after correction
        }
        else
        {
            printf("ACK %d received.\n", i);
        }
        i++;
    }

    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", transmissions);
}

void goBackN(int n, int window, int lost)
{
    int i = 1, j;
    int transmissions = 0;

    printf("\n===== GO-BACK-N =====\n");

    while (i <= n)
    {
        int end = i + window - 1;

        if (end > n)
            end = n;

        printf("\nSending window: ");

        for (j = i; j <= end; j++)
        {
            printf("%d ", j);
            transmissions++;
        }

        printf("\n");

        if (lost >= i && lost <= end)
        {
            printf("Frame %d lost/corrupted.\n", lost);

            for (j = i; j < lost; j++)
                printf("ACK %d received.\n", j);

            printf("Timeout for frame %d.\n", lost);

            printf("Retransmitting frames: ");

            for (j = lost; j <= end; j++)
            {
                printf("%d ", j);
                transmissions++;
            }

            printf("\n");

            for (j = lost; j <= end; j++)
                printf("ACK %d received.\n", j);

            lost = 0;
        }
        else
        {
            for (j = i; j <= end; j++)
                printf("ACK %d received.\n", j);
        }

        i = end + 1;
    }

    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", transmissions);
}

void selectiveRepeat(int n, int window, int lost)
{
    int i = 1, j;
    int transmissions = 0;

    printf("\n===== SELECTIVE REPEAT =====\n");

    while (i <= n)
    {
        int end = i + window - 1;

        if (end > n)
            end = n;

        printf("\nSending window: ");

        for (j = i; j <= end; j++)
        {
            printf("%d ", j);
            transmissions++;
        }

        printf("\n");

        if (lost >= i && lost <= end)
        {
            printf("Frame %d lost/corrupted.\n", lost);

            for (j = i; j <= end; j++)
            {
                if (j == lost)
                    printf("Frame %d: No ACK\n", j);
                else
                    printf("Frame %d: ACK received\n", j);
            }

            printf("Timeout for frame %d.\n", lost);

            printf("Retransmitting only frame %d\n", lost);

            transmissions++;

            printf("ACK %d received.\n", lost);

            lost = 0;
        }
        else
        {
            for (j = i; j <= end; j++)
                printf("Frame %d: ACK received\n", j);
        }

        i = end + 1;
    }

    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", transmissions);
}

int main()
{
    int choice;
    int n, window, lost;

    while (1)
    {
        window = 1; // Reset default window for Stop-and-Wait compatibility each loop iteration

        printf("\n====================================\n");
        printf("     ERROR CONTROL PROTOCOLS\n");
        printf("====================================\n");
        printf("1. Stop-and-Wait ARQ\n");
        printf("2. Go-Back-N\n");
        printf("3. Selective Repeat\n");
        printf("4. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 4)
        {
            printf("Exiting program. Goodbye!\n");
            break;
        }

        if (choice < 1 || choice > 3)
        {
            printf("Invalid choice. Please choose a valid option.\n");
            continue;
        }

        printf("Enter number of frames: ");
        scanf("%d", &n);

        // Window size input skipped for Stop-and-Wait since it is implicitly 1
        if (choice == 2 || choice == 3)
        {
            printf("Enter window size: ");
            scanf("%d", &window);

            if (window <= 0 || window > n)
            {
                printf("Invalid window size. Try again.\n");
                continue;
            }
        }

        printf("Enter frame number to be lost/corrupted (0 for none): ");
        scanf("%d", &lost);

        if (lost < 0 || lost > n)
        {
            printf("Invalid frame number. Try again.\n");
            continue;
        }

        // Execute the chosen protocol
        if (choice == 1)
        {
            stopAndWait(n, lost);
        }
        else if (choice == 2)
        {
            goBackN(n, window, lost);
        }
        else if (choice == 3)
        {
            selectiveRepeat(n, window, lost);
        }
    }

    return 0;
}
