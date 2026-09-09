#67.Add Binary
public class Solution {
    public String addBinary(String a, String b) {
        StringBuilder sb = new StringBuilder();
        int i = a.length() - 1, j = b.length() -1, carry = 0;
        while (i >= 0 || j >= 0) {
            int sum = carry;
            if (j >= 0) sum += b.charAt(j--) - '0';
            if (i >= 0) sum += a.charAt(i--) - '0';
            sb.append(sum % 2);
            carry = sum / 2;
        }
        if (carry != 0) sb.append(carry);
        return sb.reverse().toString();
    }
}

#693. Binary Numbers with alternating bits
class Solution {
    public boolean hasAlternatingBits(int n) {
        int prev = 5;   // dummy value to indicate no previous bit yet

        while(n > 0){
            int bit = n % 2;   // get last bit
            n /= 2;            // remove last bit

            if(prev == 5){
                prev = bit;    // first bit, just store it
            }else{
                if(bit == prev) return false; // same as previous  not alternating
                prev = bit;
            }
        }
        return true;
    }
}
#898 Bitwise ORs of Sub arrays
class Solution {
    public int subarrayBitwiseORs(int[] arr) {
        Set<Integer> res = new HashSet<>();
        Set<Integer> cur = new HashSet<>();

        for (int num : arr) {
            Set<Integer> next = new HashSet<>();
            next.add(num);
            for (int x : cur) {
                next.add(x | num);
            }
            cur = next;
            res.addAll(cur);
        }

        return res.size();
    }
}
[24bcsl03@mepcolinux net]$cat parity.c
#include <stdio.h>
#include <string.h>

#define MAX_LEN 100

// Function to calculate the required parity bit based on choice (0 for even, 1 for odd)
int calculate_parity(char ch, int parity_choice) {
    int count = 0;
    for (int i = 7; i >= 0; i--) {
        if ((ch >> i) & 1) {
            count++;
        }
    }

    if (parity_choice == 0) {
        // Even Parity: Total 1s (including parity bit) must be even
        return (count % 2 == 0) ? 0 : 1;
    } else {
        // Odd Parity: Total 1s (including parity bit) must be odd
        return (count % 2 != 0) ? 0 : 1;
    }
}

// Function to print the 8-bit binary representation of a character
void print_binary(char ch) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (ch >> i) & 1);
    }
}

int main() {
    char input[MAX_LEN];
    int parity_choice;
    int inject_error;

    printf("Enter a text string: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }
    // Remove trailing newline character from fgets
    input[strcspn(input, "\n")] = '\0';

    printf("Select Parity Type (0 for Even, 1 for Odd): ");
    if (scanf("%d", &parity_choice) != 1 || (parity_choice != 0 && parity_choice != 1)) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    printf("Do you want to inject an error? (0 for No Error, 1 for Inject Error): ");
    if (scanf("%d", &inject_error) != 1 || (inject_error != 0 && inject_error != 1)) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    printf("\n--- Processing and Generating Parity (%s) ---\n", parity_choice == 0 ? "Even" : "Odd");
    printf("%-6s | %-6s | %-8s | %-10s | %-12s\n", "Char", "ASCII", "Binary", "Parity Bit", "Transmitted");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; input[i] != '\0'; i++) {
        char ch = input[i];
        int ascii_val = (int)ch;
        int parity_bit = calculate_parity(ch, parity_choice);

        printf("  %c    |  %-3d   | ", ch, ascii_val);
        print_binary(ch);
        printf(" |     %d      | ", parity_bit);
        print_binary(ch);
        printf("%d\n", parity_bit);
    }

    printf("\n--- Error Detection Simulation ---\n");

    // Simulating full transmission array with its received parity bits
    char received_input[MAX_LEN];
    int received_parities[MAX_LEN];
    int length = strlen(input);

    strcpy(received_input, input);
    for(int i = 0; i < length; i++) {
        received_parities[i] = calculate_parity(input[i], parity_choice);
    }

    if (length > 0) {
        printf("Simulating checking on first character '%c':\n", received_input[0]);

        if (inject_error == 1) {
            printf("[System Status] Injecting bit error by flipping the parity bit...\n");
            received_parities[0] = !received_parities[0]; // Corrupting first character's parity
        } else {
            printf("[System Status] Keeping transmission clean...\n");
        }

        printf("   Received Bits: ");
        print_binary(received_input[0]);
        printf("%d\n", received_parities[0]);

        // Receiver counts the 1s in the received message for the first character
        int received_ones = 0;
        for (int i = 7; i >= 0; i--) {
            if ((received_input[0] >> i) & 1) received_ones++;
        }
        if (received_parities[0]) received_ones++;

        // Receiver validates parity
        int is_valid = 0;
        if (parity_choice == 0 && received_ones % 2 == 0) {
            is_valid = 1;
        } else if (parity_choice == 1 && received_ones % 2 != 0) {
            is_valid = 1;
        }

        if (is_valid) {
            printf("   Result: SUCCESS - No error detected.\n");
        } else {
            printf("   Result: FAILURE - Error detected!\n");
        }
    }

    // --- NEW: DATA EXTRACTION PHASE ---
    printf("\n--- Data Extraction Phase ---\n");

    char extracted_output[MAX_LEN];
    int extraction_failed = 0;

    for (int i = 0; i < length; i++) {
        int ones_count = 0;

        // Count 1s in the 8-bit data block
        for (int j = 7; j >= 0; j--) {
            if ((received_input[i] >> j) & 1) {
                ones_count++;
            }
        }
        // Include the trailing parity bit into total count
        if (received_parities[i]) {
            ones_count++;
        }

        // Validate chunk integrity before dropping the parity bit
        int chunk_valid = 0;
        if (parity_choice == 0 && ones_count % 2 == 0) chunk_valid = 1;
        if (parity_choice == 1 && ones_count % 2 != 0) chunk_valid = 1;

        if (chunk_valid) {
            // Parity bit is dropped; data bits are retained as the ASCII character
            extracted_output[i] = received_input[i];
        } else {
            extraction_failed = 1;
            extracted_output[i] = '?'; // Mark corrupted data point
        }
    }
    extracted_output[length] = '\0'; // Null-terminate the string

    if (extraction_failed) {
        printf("[Extraction Error] Frame dropped or contains errors. Raw Output: %s\n", extracted_output);
    } else {
        printf("[Success] All parity bits dropped successfully.\n");
        printf("Extracted Message Text: %s\n", extracted_output);
    }

    return 0;
}
[24bcsl03@mepcolinux net]$./parity
Enter a text string: arika
Select Parity Type (0 for Even, 1 for Odd): 0
Do you want to inject an error? (0 for No Error, 1 for Inject Error): 0

--- Processing and Generating Parity (Even) ---
Char   | ASCII  | Binary   | Parity Bit | Transmitted
-------------------------------------------------------------
  a    |  97    | 01100001 |     1      | 011000011
  r    |  114   | 01110010 |     0      | 011100100
  i    |  105   | 01101001 |     0      | 011010010
  k    |  107   | 01101011 |     1      | 011010111
  a    |  97    | 01100001 |     1      | 011000011

--- Error Detection Simulation ---
Simulating checking on first character 'a':
[System Status] Keeping transmission clean...
   Received Bits: 011000011
   Result: SUCCESS - No error detected.

--- Data Extraction Phase ---
[Success] All parity bits dropped successfully.
Extracted Message Text: arika
[24bcsl03@mepcolinux net]$./parity
Enter a text string: arika
Select Parity Type (0 for Even, 1 for Odd): 1
Do you want to inject an error? (0 for No Error, 1 for Inject Error): 1

--- Processing and Generating Parity (Odd) ---
Char   | ASCII  | Binary   | Parity Bit | Transmitted
-------------------------------------------------------------
  a    |  97    | 01100001 |     0      | 011000010
  r    |  114   | 01110010 |     1      | 011100101
  i    |  105   | 01101001 |     1      | 011010011
  k    |  107   | 01101011 |     0      | 011010110
  a    |  97    | 01100001 |     0      | 011000010

--- Error Detection Simulation ---
Simulating checking on first character 'a':
[System Status] Injecting bit error by flipping the parity bit...
   Received Bits: 011000011
   Result: FAILURE - Error detected!

--- Data Extraction Phase ---
[Extraction Error] Frame dropped or contains errors. Raw Output: ?rika
[24bcsl03@mepcolinux net]$cat checksum.c
#include <stdio.h>
#include <string.h>

#define MAXBITS 1000
#define MAXBLOCKS 200

char input[MAXBITS];
char padded[MAXBITS];
char data[MAXBLOCKS][9];
char checksum[9];

int n;      // number of blocks
int pad;    // number of padded zeros

//--------------------------------------------------
// Binary Addition with End Around Carry
//--------------------------------------------------
void binaryAdd(char a[], char b[], char sum[])
{
    int carry = 0;

    for(int i=7;i>=0;i--)
    {
        int bit1 = a[i]-'0';
        int bit2 = b[i]-'0';

        int s = bit1 + bit2 + carry;

        sum[i]=(s%2)+'0';
        carry=s/2;
    }

    while(carry)
    {
        for(int i=7;i>=0;i--)
        {
            int bit=sum[i]-'0';
            int s=bit+carry;

            sum[i]=(s%2)+'0';
            carry=s/2;
        }
    }

    sum[8]='\0';
}

//--------------------------------------------------
// 1's Complement
//--------------------------------------------------
void complement(char str[])
{
    for(int i=0;i<8;i++)
    {
        if(str[i]=='0')
            str[i]='1';
        else
            str[i]='0';
    }
}

//--------------------------------------------------
// Convert Input into 8-bit Blocks
//--------------------------------------------------
void divideBlocks()
{
    int len=strlen(input);

    pad=(8-(len%8))%8;

    int k=0;

    // add leading zeros
    for(int i=0;i<pad;i++)
        padded[k++]='0';

    // copy original bits
    for(int i=0;i<len;i++)
        padded[k++]=input[i];

    padded[k]='\0';

    printf("\nPadded Data : %s\n",padded);

    n=k/8;

    printf("\n8-bit Blocks\n");

    for(int i=0;i<n;i++)
    {
        strncpy(data[i],padded+i*8,8);
        data[i][8]='\0';

        printf("Block %d : %s\n",i+1,data[i]);
    }
}

//--------------------------------------------------
// Generate Checksum
//--------------------------------------------------
void generateChecksum()
{
    char sum[9]="00000000";

    printf("\nSender Side Addition\n");

    for(int i=0;i<n;i++)
    {
        printf("%s\n",data[i]);
        binaryAdd(sum,data[i],sum);
    }

    printf("----------------\n");
    printf("Sum      : %s\n",sum);

    strcpy(checksum,sum);

    complement(checksum);

    printf("Checksum : %s\n",checksum);
}

//--------------------------------------------------
// Verify Receiver
//--------------------------------------------------
void verify(char recv[][9])
{
    char sum[9]="00000000";

    printf("\nReceiver Side Addition\n");

    for(int i=0;i<n;i++)
    {
        printf("%s\n",recv[i]);
        binaryAdd(sum,recv[i],sum);
    }

    printf("%s (Checksum)\n",checksum);

    binaryAdd(sum,checksum,sum);

    printf("----------------\n");
    printf("Final Sum             : %s\n",sum);

    complement(sum);

    printf("1's Complement        : %s\n",sum);

    if(strcmp(sum,"00000000")==0)
    {
        printf("\nNO ERROR DETECTED\n");
    }
    else
    {
        printf("\nERROR DETECTED\n");
        printf("MESSAGE DISCARDED\n");
    }
}

//--------------------------------------------------
// Main
//--------------------------------------------------
int main()
{
    char recv[MAXBLOCKS][9];

    int choice;

    printf("Enter Binary Data : ");
    scanf("%s",input);

    divideBlocks();

    generateChecksum();

    printf("\n\nTransmitted Data\n");

    for(int i=0;i<n;i++)
        printf("%s ",data[i]);

    printf("%s",checksum);

    while(1)
    {
        for(int i=0;i<n;i++)
            strcpy(recv[i],data[i]);

        printf("\n\n------ MENU ------\n");
        printf("1. No Error\n");
        printf("2. Single Bit Error\n");
        printf("3. Multiple Bit Error\n");
        printf("4. Exit\n");

        printf("Enter Choice : ");
        scanf("%d",&choice);

        if(choice==1)
        {
            verify(recv);
        }

        else if(choice==2)
        {
            int block,pos;

            printf("Enter Block Number : ");
            scanf("%d",&block);

            printf("Enter Bit Position (1-8) : ");
            scanf("%d",&pos);

            recv[block-1][pos-1]=(recv[block-1][pos-1]=='0')?'1':'0';

            verify(recv);
        }

        else if(choice==3)
        {
            int block1,pos1,block2,pos2;

            printf("Enter First Block Number : ");
            scanf("%d",&block1);

            printf("Enter First Bit Position : ");
            scanf("%d",&pos1);

            printf("Enter Second Block Number : ");
            scanf("%d",&block2);

            printf("Enter Second Bit Position : ");
            scanf("%d",&pos2);

            recv[block1-1][pos1-1]=(recv[block1-1][pos1-1]=='0')?'1':'0';
            recv[block2-1][pos2-1]=(recv[block2-1][pos2-1]=='0')?'1':'0';

            verify(recv);
        }

        else if(choice==4)
        {
            break;
        }

        else
        {
            printf("Invalid Choice\n");
        }
    }

    return 0;
}
[24bcsl03@mepcolinux net]$./checksum
Enter Binary Data : 101101010101

Padded Data : 0000101101010101

8-bit Blocks
Block 1 : 00001011
Block 2 : 01010101

Sender Side Addition
00001011
01010101
----------------
Sum      : 01100000
Checksum : 10011111


Transmitted Data
00001011 01010101 10011111

------ MENU ------
1. No Error
2. Single Bit Error
3. Multiple Bit Error
4. Exit
Enter Choice : 1

Receiver Side Addition
00001011
01010101
10011111 (Checksum)
----------------
Final Sum             : 11111111
1's Complement        : 00000000

NO ERROR DETECTED


------ MENU ------
1. No Error
2. Single Bit Error
3. Multiple Bit Error
4. Exit
Enter Choice : 2
Enter Block Number : 1
Enter Bit Position (1-8) : 2

Receiver Side Addition
01001011
01010101
10011111 (Checksum)
----------------
Final Sum             : 01000000
1's Complement        : 10111111

ERROR DETECTED
MESSAGE DISCARDED


------ MENU ------
1. No Error
2. Single Bit Error
3. Multiple Bit Error
4. Exit
Enter Choice : 3
Enter First Block Number : 1
Enter First Bit Position : 2
Enter Second Block Number : 1
Enter Second Bit Position : 3

Receiver Side Addition
01101011
01010101
10011111 (Checksum)
----------------
Final Sum             : 01100000
1's Complement        : 10011111

ERROR DETECTED
MESSAGE DISCARDED


------ MENU ------
1. No Error
2. Single Bit Error
3. Multiple Bit Error
4. Exit
Enter Choice : 4
[24bcsl03@mepcolinux net]$cat crc.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define MAX_BITS 1024

void ascii_to_bin(char ch, char *b) {
    for (int i = 7; i >= 0; i--) {
        b[7 - i] = (ch & (1 << i)) ? '1' : '0';
    }
    b[8] = '\0';
}

char bin_to_ascii(const char *b) {
    char ch = 0;
    for (int i = 0; i < 8; i++) {
        ch = (ch << 1) | (b[i] - '0');
    }
    return ch;
}

void str_to_bits(const char *str, char *bits) {
    int len = strlen(str);
    char b[9];
    for (int i = 0; i < len; i++) {
        ascii_to_bin(str[i], b);
        strcpy(bits + (i * 8), b);
    }
    bits[len * 8] = '\0';
}

void bits_to_str(const char *bits, char *str) {
    int nchars = strlen(bits) / 8;
    for (int i = 0; i < nchars; i++) {
        str[i] = bin_to_ascii(bits + (i * 8));
    }
    str[nchars] = '\0';
}

void simulate_error(char *bits) {
    int len = strlen(bits);
    int pos;
    printf("\nSimulate a transmission error?\n");
    printf("Enter bit position to flip (0 to %d), or -1 for NO error: ", len - 1);
    scanf("%d", &pos);
    if (pos >= 0 && pos < len) {
        bits[pos] = (bits[pos] == '0') ? '1' : '0';
        printf("Bit at position %d flipped.\n", pos);
    } else {
        printf("No error introduced.\n");
    }
}

void xor_div(const char *data, const char *gen, char *remainder) {
    int data_len = strlen(data);
    int gen_len = strlen(gen);
    char *temp = (char *)malloc(data_len + 1);
    strcpy(temp, data);
    for (int i = 0; i <= data_len - gen_len; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < gen_len; j++) {
                temp[i + j] = (((temp[i + j] - '0') ^ (gen[j] - '0')) + '0');
            }
        }
    }
    strcpy(remainder, temp + (data_len - (gen_len - 1)));
    free(temp);
}

int main() {
    char input[MAX_STR];
    char data[MAX_BITS], gen[32];
    char dividend[MAX_BITS], remainder[32], transmitted[MAX_BITS];

    printf("\n---- CRC : SENDER SIDE ----\n");
    printf("Enter string message: ");
    scanf("%s", input);
    str_to_bits(input, data);
    printf("Source data (binary) : %s\n", data);

    printf("Enter generator polynomial bits (e.g. 1001), or press 0 for default 1001: ");
    scanf("%s", gen);
    if (strcmp(gen, "0") == 0) strcpy(gen, "1001");
    printf("Generator G(x) used : %s\n", gen);

    /* Check for a good generator */
    int gen_len = strlen(gen);

    // Conditions:
    // 1. Minimum 2 bits (gen_len >= 2)
    // 2. x^0 coefficient is 1 (gen[gen_len - 1] == '1')
    // 3. Last two bits of the generator are '1' (gen[gen_len - 1] == '1' && gen[gen_len - 2] == '1')
    if (gen_len >= 2 && gen[gen_len - 1] == '1' && gen[gen_len - 2] == '1') {
        printf("Generator quality : GOOD GENERATOR\n");
    } else {
        printf("Generator quality : NOT A GOOD GENERATOR\n");
        return 0;
    }

    int r = strlen(gen) - 1;
    strcpy(dividend, data);
    for (int i = 0; i < r; i++) dividend[strlen(data) + i] = '0';
    dividend[strlen(data) + r] = '\0';
    printf("Data padded with %d zeros : %s\n", r, dividend);

    xor_div(dividend, gen, remainder);
    printf("CRC remainder (redundant bits) : %s\n", remainder);

    strcpy(transmitted, data);
    strcat(transmitted, remainder);
    printf("Data to be transmitted (T = D+CRC): %s\n", transmitted);

    printf("\n---- CRC : RECEIVER SIDE ----\n");
    char received[MAX_BITS];
    strcpy(received, transmitted);
    simulate_error(received);
    printf("Received data : %s\n", received);

    char check_remainder[32];
    xor_div(received, gen, check_remainder);
    printf("Remainder after division by G(x) : %s\n", check_remainder);

    int all_zero = 1;
    for (int i = 0; i < (int)strlen(check_remainder); i++) {
        if (check_remainder[i] != '0') all_zero = 0;
    }

    if (all_zero) {
        printf("Result : NO ERROR DETECTED\n");
        char recovered_bits[MAX_BITS], recovered_str[MAX_STR];
        strncpy(recovered_bits, received, strlen(data));
        recovered_bits[strlen(data)] = '\0';
        bits_to_str(recovered_bits, recovered_str);
        printf("Recovered message (string) : %s\n", recovered_str);
    } else {
        printf("Result : ERROR DETECTED\n");
    }
    return 0;
}
[24bcsl03@mepcolinux net]$./crc

---- CRC : SENDER SIDE ----
Enter string message: ari
Source data (binary) : 011000010111001001101001
Enter generator polynomial bits (e.g. 1001), or press 0 for default 1001: 10011
Generator G(x) used : 10011
Generator quality : GOOD GENERATOR
Data padded with 4 zeros : 0110000101110010011010010000
CRC remainder (redundant bits) : 1001
Data to be transmitted (T = D+CRC): 0110000101110010011010011001

---- CRC : RECEIVER SIDE ----

Simulate a transmission error?
Enter bit position to flip (0 to 27), or -1 for NO error: -1
No error introduced.
Received data : 0110000101110010011010011001
Remainder after division by G(x) : 0000
Result : NO ERROR DETECTED
Recovered message (string) : ari
[24bcsl03@mepcolinux net]$./crc

---- CRC : SENDER SIDE ----
Enter string message: ari
Source data (binary) : 011000010111001001101001
Enter generator polynomial bits (e.g. 1001), or press 0 for default 1001: 10011
Generator G(x) used : 10011
Generator quality : GOOD GENERATOR
Data padded with 4 zeros : 0110000101110010011010010000
CRC remainder (redundant bits) : 1001
Data to be transmitted (T = D+CRC): 0110000101110010011010011001

---- CRC : RECEIVER SIDE ----

Simulate a transmission error?
Enter bit position to flip (0 to 27), or -1 for NO error: 3
Bit at position 3 flipped.
Received data : 0111000101110010011010011001
Remainder after division by G(x) : 1010
Result : ERROR DETECTED
[24bcsl03@mepcolinux net]$./crc

---- CRC : SENDER SIDE ----
Enter string message: ari
Source data (binary) : 011000010111001001101001
Enter generator polynomial bits (e.g. 1001), or press 0 for default 1001: 10101010
Generator G(x) used : 10101010
Generator quality : NOT A GOOD GENERATOR
[24bcsl03@mepcolinux net]$exit
exit

Script done on Wed Aug 19 14:51:33 2026
