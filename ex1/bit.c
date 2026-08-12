#include <stdio.h>
#include <string.h>

int main()
{
    char text[100];
    char binary[1000] = "";
    char stuffed[1500], destuffed[1000];
    char ch;
    int i, j, count;
    int pos, pos1, pos2, error_type, len;

    printf("Enter Text Data : ");
    scanf("%s", text);
    for(i = 0; text[i] != '\0'; i++)
    {
        for(j = 7; j >= 0; j--)
        {
            char bit = ((text[i] >> j) & 1) + '0';
            int l = strlen(binary);
            binary[l] = bit;
            binary[l + 1] = '\0';
        }
    }
    j = 0;
    count = 0;

    for(i = 0; binary[i] != '\0'; i++)
    {
        stuffed[j++] = binary[i];

        if(binary[i] == '1')
        {
            count++;

            if(count == 5)
            {
                stuffed[j++] = '0';
                count = 0;
            }
        }
        else
        {
            count = 0;
        }
    }
    stuffed[j] = '\0';
    printf("\nBinary Data: %s", binary);
    printf("\nStuffed Data: %s", stuffed);
    printf("\nTransmitted Frame : ");
    printf("01111110 %s 01111110\n", stuffed);

    printf("\nDo you want to induce an error? (Y/N) : ");
    scanf(" %c", &ch);

    if(ch == 'Y' || ch == 'y')
    {
        len = strlen(stuffed);
printf("Choose Error Type:\n1. Single-bit error\n2. Multiple-bit error\nEnter choice (1 or 2): ");        scanf("%d", &error_type);

       if(error_type == 1)
{
    printf("Enter Bit Position (1-%d) : ", len);
    scanf("%d", &pos);

    if(pos >= 1 && pos <= len)
    {
        stuffed[pos - 1] = (stuffed[pos - 1] == '0') ? '1' : '0';
        printf("\nBit at Position %d flipped.", pos);
        printf("\n\nFrame After Single-Bit Error : %s", stuffed);
        printf("\nTransmission Error Detected! Frame Discarded.");
    }
    else
    {
        printf("\nInvalid Bit Position.\n");
    }
}
else if(error_type == 2)
{
    int numBits, positions[100];

    printf("Enter Number of Bits to Flip : ");
    scanf("%d", &numBits);

    if(numBits <= 0 || numBits > len)
    {
        printf("\nInvalid Number of Bits.\n");
    }
    else
    {
        for(i = 0; i < numBits; i++)
        {
            printf("Enter Bit Position %d (1-%d) : ", i + 1, len);
            scanf("%d", &positions[i]);

            if(positions[i] >= 1 && positions[i] <= len)
            {
                stuffed[positions[i] - 1] =
                    (stuffed[positions[i] - 1] == '0') ? '1' : '0';
            }
            else
            {
                printf("Invalid Position %d ignored.\n", positions[i]);
            }
        }

        printf("\n%d Bits Flipped.", numBits);
        printf("\n\nFrame After Multiple-Bit Error : %s", stuffed);
        printf("\nTransmission Error Detected! Frame Discarded.");
    }
}
    else
    {
        printf("\nNo Error Introduced.");
        count = 0;
        j = 0;
        for(i = 0; stuffed[i] != '\0'; i++)
        {
            destuffed[j++] = stuffed[i];
            if(stuffed[i] == '1')
            {
                count++;
                if(count == 5)
                {
                    i++;
                    count = 0;
                }
            }
            else
            {
                count = 0;
            }
        }
        destuffed[j] = '\0';
        printf("\n\nReceiver Output : %s\n", destuffed);
    }
    return 0;
    }
}