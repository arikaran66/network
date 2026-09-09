#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 100

int code[MAX];
int filled[MAX];
char label[MAX][4];

int isPowerOf2(int pos)
{
    return pos > 0 && (pos & (pos - 1)) == 0;
}


int findParityBits(int n)
{
    int p = 1;
    printf("\nStep 2:\nFinding parity bits (p)\n");
    while (1)
    {
        printf("\nTry p = %d\n%d + %d + 1 <= 2^%d  ", p, n, p, p);
        if (n + p + 1 <= (int)pow(2, p))
        {
            printf("-> Yes\n");
            break;
        }
        else
        {
            printf("-> No\n");
            p++;
        }
    }
    printf("\nRequired parity bits = %d\n", p);
    return p;
}

void assignLabels(int totalLen, int p)
{
    int n = totalLen - p;
    int dNum = n;
    for (int j = totalLen; j >= 1; j--)
    {
        if (isPowerOf2(j))
        {
            int k = (int)(log2(j)) + 1;
            sprintf(label[j], "P%d", k);
        }
        else
        {
            sprintf(label[j], "D%d", dNum);
            dNum--;
        }
    }
}
void placeDataBits(const char *data, int totalLen)
{
    int idx = 0;
    for (int j = totalLen; j >= 1; j--)
    {
        if (!isPowerOf2(j))
        {
            code[j] = data[idx] - '0';
            filled[j] = 1;
            idx++;
        }
        else
        {
            code[j] = 0;
            filled[j] = 0;
        }
    }
}
void displayFrame(int totalLen)
{
    printf("\nPosition :\n");
    for (int j = totalLen; j >= 1; j--)
        printf("%3d", j);

    printf("\n\nType :\n");
    for (int j = totalLen; j >= 1; j--)
        printf("%3s", label[j]);

    printf("\n\nValue :\n");
    for (int j = totalLen; j >= 1; j--)
    {
        if (filled[j])
            printf("%3d", code[j]);
        else
            printf("  _");
    }
    printf("\n");
}
void calculateParity(int totalLen, int p, int parityType)
{
    printf("\nStep 4:\nCalculating parity bits (%s parity)\n",
           parityType == 0 ? "Even" : "Odd");

    for (int k = 1; k <= p; k++)
    {
        int parityPos = 1 << (k - 1);
        if (parityPos > totalLen)
            continue;

        printf("\nCalculate P%d\n\nChecking positions\n", k);
        for (int j = 1; j <= totalLen; j++)
            if (j & parityPos)
                printf("%d ", j);

        printf("\n\nValues\n");
        int count = 0;
        for (int j = 1; j <= totalLen; j++)
        {
            if (j & parityPos)
            {
                if (j == parityPos)
                    printf("_ ");
                else
                {
                    printf("%d ", code[j]);
                    count += code[j];
                }
            }
        }

        int bit;
        if (parityType == 0)
            bit = (count % 2 == 0) ? 0 : 1; /* even */
        else
            bit = (count % 2 == 0) ? 1 : 0; /* odd  */

        code[parityPos] = bit;
        filled[parityPos] = 1;

        printf("\n\nP%d = %d\n", k, bit);
    }
}

void generateHammingCode(const char *data, int parityType)
{
    int n = strlen(data);
    printf("\nStep 1:\nNumber of data bits (n) = %d\n", n);

    int p = findParityBits(n);
    int totalLen = n + p;
    printf("Total bits = %d\n", totalLen);

    assignLabels(totalLen, p);
    placeDataBits(data, totalLen);

    printf("\nStep 3:\nInsert empty parity locations\n");
    displayFrame(totalLen);

    calculateParity(totalLen, p, parityType);

    printf("\nFinal Hamming Code\n");
    displayFrame(totalLen);

    printf("\n\nData to be transmitted: ");
    for (int j = totalLen; j >= 1; j--)
        printf("%d", code[j]);
    printf("\n");
}

int detectError(int totalLen, int p, int parityType)
{
    int syndrome = 0;
    printf("\nChecking received code:\n");

    for (int k = p; k >= 1; k--)
    {
        int parityPos = 1 << (k - 1);
        if (parityPos > totalLen)
            continue;

        int count = 0;
        for (int j = 1; j <= totalLen; j++)
            if (j & parityPos)
                count += code[j]; /* parity bit itself included */

        int checkBit;
        if (parityType == 0)
            checkBit = (count % 2 == 0) ? 0 : 1;
        else
            checkBit = (count % 2 == 0) ? 1 : 0;

        printf("Checking P%d ... = %d\n", k, checkBit);
        if (checkBit)
            syndrome += parityPos;
    }

    printf("\nSyndrome (binary value of check bits) = %d\n", syndrome);
    return syndrome;
}

void correctError(int errorPos, int totalLen)
{
    if (errorPos == 0)
    {
        printf("\nNo error detected.\n");
    }
    else if (errorPos > totalLen)
    {
        printf("\nError position out of range - more than 1 bit may be corrupted.\n");
    }
    else
    {
        printf("\nError found at Position %d -> flipping bit to correct.\n", errorPos);
        code[errorPos] = code[errorPos] ? 0 : 1;
    }
    printf("\nCorrected Code\n");
    displayFrame(totalLen);
    printf("\n");
}

void extractData(int totalLen)
{
    printf("\nOriginal Data: ");
    for (int j = totalLen; j >= 1; j--)
        if (!isPowerOf2(j))
            printf("%d", code[j]);
    printf("\n");
}

int main()
{
    int choice, parityType, totalLen, p;
    char input[MAX];

    printf("=== HAMMING CODE PROGRAM ===\n");
    printf("1. Encode data (generate Hamming code)\n");
    printf("2. Check received code for error and correct it\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    printf("Enter parity type (0 = Even, 1 = Odd): ");
    scanf("%d", &parityType);

    if (choice == 1)
    {
        printf("Enter data bits (e.g. 1011010): ");
        scanf("%s", input);
        generateHammingCode(input, parityType);
    }
    else if (choice == 2)
    {
        printf("Enter received code (position N ... position 1, e.g. 10101010111): ");
        scanf("%s", input);

        totalLen = strlen(input);
        p = 0;
        while ((int)pow(2, p) < totalLen + 1)
            p++;

        assignLabels(totalLen, p);
        for (int j = totalLen, idx = 0; j >= 1; j--, idx++)
        {
            code[j] = input[idx] - '0';
            filled[j] = 1;
        }

        printf("\nReceived Code\n");
        displayFrame(totalLen);

        int syndrome = detectError(totalLen, p, parityType);
        correctError(syndrome, totalLen);
        extractData(totalLen);
    }
    else
    {
        printf("Invalid choice.\n");
    }

    return 0;
}
[24bcsl03@mepcolinux ex3]$./ham
=== HAMMING CODE PROGRAM ===
1. Encode data (generate Hamming code)
2. Check received code for error and correct it
Enter choice: 1
Enter parity type (0 = Even, 1 = Odd): 0
Enter data bits (e.g. 1011010): 1011010

Step 1:
Number of data bits (n) = 7

Step 2:
Finding parity bits (p)

Try p = 1
7 + 1 + 1 <= 2^1  -> No

Try p = 2
7 + 2 + 1 <= 2^2  -> No

Try p = 3
7 + 3 + 1 <= 2^3  -> No

Try p = 4
7 + 4 + 1 <= 2^4  -> Yes

Required parity bits = 4
Total bits = 11

Step 3:
Insert empty parity locations

Position :
 11 10  9  8  7  6  5  4  3  2  1

Type :
 D7 D6 D5 P4 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  _  1  0  1  _  0  _  _

Step 4:
Calculating parity bits (Even parity)

Calculate P1

Checking positions
1 3 5 7 9 11

Values
_ 0 1 1 1 1

P1 = 0

Calculate P2

Checking positions
2 3 6 7 10 11

Values
_ 0 0 1 0 1

P2 = 0

Calculate P3

Checking positions
4 5 6 7

Values
_ 1 0 1

P3 = 0

Calculate P4

Checking positions
8 9 10 11

Values
_ 1 0 1

P4 = 0

Final Hamming Code

Position :
 11 10  9  8  7  6  5  4  3  2  1

Type :
 D7 D6 D5 P4 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  0  1  0  1  0  0  0  0


Data to be transmitted: 10101010000
[24bcsl03@mepcolinux ex3]$./ham
=== HAMMING CODE PROGRAM ===
1. Encode data (generate Hamming code)
2. Check received code for error and correct it
Enter choice: 1
Enter parity type (0 = Even, 1 = Odd): 1
Enter data bits (e.g. 1011010): 1011

Step 1:
Number of data bits (n) = 4

Step 2:
Finding parity bits (p)

Try p = 1
4 + 1 + 1 <= 2^1  -> No

Try p = 2
4 + 2 + 1 <= 2^2  -> No

Try p = 3
4 + 3 + 1 <= 2^3  -> Yes

Required parity bits = 3
Total bits = 7

Step 3:
Insert empty parity locations

Position :
  7  6  5  4  3  2  1

Type :
 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  _  1  _  _

Step 4:
Calculating parity bits (Odd parity)

Calculate P1

Checking positions
1 3 5 7

Values
_ 1 1 1

P1 = 0

Calculate P2

Checking positions
2 3 6 7

Values
_ 1 0 1

P2 = 1

Calculate P3

Checking positions
4 5 6 7

Values
_ 1 0 1

P3 = 1

Final Hamming Code

Position :
  7  6  5  4  3  2  1

Type :
 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  1  1  1  0


Data to be transmitted: 1011110
[24bcsl03@mepcolinux ex3]$./ham
=== HAMMING CODE PROGRAM ===
1. Encode data (generate Hamming code)
2. Check received code for error and correct it
Enter choice: 2
Enter parity type (0 = Even, 1 = Odd): 0
Enter received code (position N ... position 1, e.g. 10101010111): 10101110000

Received Code

Position :
 11 10  9  8  7  6  5  4  3  2  1

Type :
 D7 D6 D5 P4 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  0  1  1  1  0  0  0  0

Checking received code:
Checking P4 ... = 0
Checking P3 ... = 1
Checking P2 ... = 1
Checking P1 ... = 0

Syndrome (binary value of check bits) = 6

Error found at Position 6 -> flipping bit to correct.

Corrected Code

Position :
 11 10  9  8  7  6  5  4  3  2  1

Type :
 D7 D6 D5 P4 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  0  1  0  1  0  0  0  0


Original Data: 1011010
[24bcsl03@mepcolinux ex3]$./ham
=== HAMMING CODE PROGRAM ===
1. Encode data (generate Hamming code)
2. Check received code for error and correct it
Enter choice: 2
Enter parity type (0 = Even, 1 = Odd): 1001110
Enter received code (position N ... position 1, e.g. 10101010111): ^C
[24bcsl03@mepcolinux ex3]$./ham
=== HAMMING CODE PROGRAM ===
1. Encode data (generate Hamming code)
2. Check received code for error and correct it
Enter choice: 2
Enter parity type (0 = Even, 1 = Odd): 1
Enter received code (position N ... position 1, e.g. 10101010111): 1001110

Received Code

Position :
  7  6  5  4  3  2  1

Type :
 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  0  1  1  1  0

Checking received code:
Checking P3 ... = 1
Checking P2 ... = 0
Checking P1 ... = 1

Syndrome (binary value of check bits) = 5

Error found at Position 5 -> flipping bit to correct.

Corrected Code

Position :
  7  6  5  4  3  2  1

Type :
 D4 D3 D2 P3 D1 P2 P1

Value :
  1  0  1  1  1  1  0


Original Data: 1011
[24bcsl03@mepcolinux ex3]$exit
exit

Script done on Wed Aug 19 15:35:55 2026
[24bcsl03@mepcolinux ex3]$
