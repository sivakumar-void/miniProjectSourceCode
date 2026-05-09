#include <stdio.h>gitt
#include <stdlib.h>
#include <string.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    char password[10];
};

// function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);

int checkPassword(struct clientData client)
{
    char pass[10];
    printf("Enter password: ");
    scanf("%9s", pass);

    if (strcmp(pass, client.password) == 0)
        return 1;

    printf("Wrong password!\n");
    return 0;
}

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    // open file
    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        printf("File not found! Creating new file...\n");

        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be created.\n");
            return 1;
        }

        struct clientData blank = {0, "", "", 0.0, ""};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            displayAll(cfPtr);
            break;
        case 6:
            searchAccount(cfPtr);
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n====== BANK MENU ======\n");
    printf("1 - Export to text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Display all accounts\n");
    printf("6 - Search account\n");
    printf("7 - Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

// export to text
void textFile(FILE *readPtr)
{
    FILE *writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Error creating file\n");
        return;
    }

    struct clientData client;
    rewind(readPtr);

    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    printf("Data exported to accounts.txt\n");
}

// display all
void displayAll(FILE *fPtr)
{
    struct clientData client;
    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// search account
void searchAccount(FILE *fPtr)
{
    unsigned int account;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Read error\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account not found\n");
        return;
    }

    if (!checkPassword(client)) return;

    printf("\nAccount Found:\n");
    printf("Account: %u\nName: %s %s\nBalance: %.2f\n",
           client.acctNum,
           client.firstName,
           client.lastName,
           client.balance);
}

// update record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Read error\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account not found\n");
        return;
    }

    if (!checkPassword(client)) return;

    printf("Current balance: %.2f\n", client.balance);
    printf("Enter amount (+ deposit, - withdraw): ");
    scanf("%lf", &transaction);

    if (client.balance + transaction < 0)
    {
        printf("Insufficient balance!\n");
        return;
    }

    client.balance += transaction;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated balance: %.2f\n", client.balance);
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, ""};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists\n");
        return;
    }

    printf("Enter last name, first name, balance:\n");
    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    printf("Set password: ");
    scanf("%9s", client.password);

    client.acctNum = account;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully!\n");
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0, "", "", 0.0, ""};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Read error\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account does not exist\n");
        return;
    }

    if (!checkPassword(client)) return;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully\n");
}