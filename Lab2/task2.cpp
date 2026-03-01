#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

class BankAccount
{
protected:
    int accountIdentifier;
    string accountHolderName;
    double currentBalance;
    unsigned int permissionFlags;
    vector<double> transactionHistory;
    vector<unsigned int> compressedTransactions;

public:
    BankAccount(int id, string name, double balance, unsigned int permissions)
    {
        accountIdentifier = id;
        accountHolderName = name;
        currentBalance = balance;
        permissionFlags = permissions;
    }

    virtual void depositAmount(double depositValue) = 0;
    virtual void withdrawAmount(double withdrawValue) = 0;
    virtual string getAccountType() = 0;

    int getAccountId()
    {
        return accountIdentifier;
    }

    string getAccountHolder()
    {
        return accountHolderName;
    }

    double getBalance()
    {
        return currentBalance;
    }

    unsigned int getPermissions()
    {
        return permissionFlags;
    }

    void displayAccountDetails()
    {
        cout << "Account ID: " << accountIdentifier << endl;
        cout << "Name: " << accountHolderName << endl;
        cout << "Balance: " << currentBalance << endl;
        cout << "Permissions: " << permissionFlags << endl;
        cout << "Transactions: ";
        for (int i = 0; i < transactionHistory.size(); i++)
            cout << transactionHistory[i] << " ";
        cout << endl;
    }

    unsigned int encodeTransaction(unsigned int transactionType, unsigned int amount)
    {
        unsigned int encodedValue = (transactionType << 28) | (amount & 0x0FFFFFFF);
        return encodedValue;
    }

    void decodeTransaction(unsigned int encodedValue, unsigned int &transactionType, unsigned int &amount)
    {
        transactionType = encodedValue >> 28;
        amount = encodedValue & 0x0FFFFFFF;
    }

    void saveAccountToFile(ofstream &outputFileStream)
    {
        outputFileStream << "ACCOUNT " << getAccountType() << endl;
        outputFileStream << accountIdentifier << " "
                         << accountHolderName << " "
                         << currentBalance << " "
                         << permissionFlags << endl;

        outputFileStream << "TRANSACTIONS" << endl;
        for (int i = 0; i < transactionHistory.size(); i++)
            outputFileStream << transactionHistory[i] << endl;
    }

    virtual ~BankAccount() {}
};

class SavingsBankAccount : public BankAccount
{
public:
    SavingsBankAccount(int id, string name, double balance, unsigned int permissions)
        : BankAccount(id, name, balance, permissions) {}

    void depositAmount(double depositValue)
    {
        if (permissionFlags & 2)
        {
            currentBalance += depositValue;
            transactionHistory.push_back(depositValue);
            compressedTransactions.push_back(encodeTransaction(1, (unsigned int)depositValue));
        }
        else
            cout << "Deposit permission denied.\n";
    }

    void withdrawAmount(double withdrawValue)
    {
        if (permissionFlags & 1)
        {
            if (withdrawValue <= currentBalance)
            {
                currentBalance -= withdrawValue;
                transactionHistory.push_back(-withdrawValue);
                compressedTransactions.push_back(encodeTransaction(2, (unsigned int)withdrawValue));
            }
            else
                cout << "Insufficient balance.\n";
        }
        else
            cout << "Withdraw permission denied.\n";
    }

    string getAccountType()
    {
        return "Savings";
    }
};

class CurrentBankAccount : public BankAccount
{
public:
    CurrentBankAccount(int id, string name, double balance, unsigned int permissions)
        : BankAccount(id, name, balance, permissions) {}

    void depositAmount(double depositValue)
    {
        if (permissionFlags & 2)
        {
            currentBalance += depositValue;
            transactionHistory.push_back(depositValue);
            compressedTransactions.push_back(encodeTransaction(1, (unsigned int)depositValue));
        }
        else
            cout << "Deposit permission denied.\n";
    }

    void withdrawAmount(double withdrawValue)
    {
        if (permissionFlags & 1)
        {
            currentBalance -= withdrawValue;
            transactionHistory.push_back(-withdrawValue);
            compressedTransactions.push_back(encodeTransaction(2, (unsigned int)withdrawValue));
        }
        else
            cout << "Withdraw permission denied.\n";
    }

    string getAccountType()
    {
        return "Current";
    }
};

void saveAllAccounts(vector<BankAccount*> &allAccounts)
{
    ofstream outputFile("bankdata.txt");

    for (int i = 0; i < allAccounts.size(); i++)
        allAccounts[i]->saveAccountToFile(outputFile);

    outputFile.close();
}

void calculateMonthlySummary(vector<BankAccount*> &allAccounts)
{
    double monthlyTotals[12] = {0};

    for (int i = 0; i < allAccounts.size(); i++)
    {
        for (int j = 0; j < 12 && j < allAccounts[i]->getBalance(); j++)
            monthlyTotals[j] += 0;
    }

    cout << "Monthly Summary:\n";
    for (int i = 0; i < 12; i++)
        cout << "Month " << i + 1 << ": " << monthlyTotals[i] << endl;
}

int main()
{
    vector<BankAccount*> bankAccountsCollection;
    int userChoice;

    while (true)
    {
        cout << "\n1.Create Account\n2.Deposit\n3.Withdraw\n4.Show Account\n5.Save\n6.Exit\nChoice: ";
        cin >> userChoice;

        if (userChoice == 1)
        {
            int id;
            string name;
            double balance;
            unsigned int permissions;
            int accountTypeChoice;

            cout << "1.Savings 2.Current: ";
            cin >> accountTypeChoice;

            cout << "Enter ID Name Balance Permissions: ";
            cin >> id >> name >> balance >> permissions;

            BankAccount* newAccountPointer;

            if (accountTypeChoice == 1)
                newAccountPointer = new SavingsBankAccount(id, name, balance, permissions);
            else
                newAccountPointer = new CurrentBankAccount(id, name, balance, permissions);

            bankAccountsCollection.push_back(newAccountPointer);
        }

        else if (userChoice == 2)
        {
            int id;
            double amount;
            cout << "Enter ID and Amount: ";
            cin >> id >> amount;

            for (int i = 0; i < bankAccountsCollection.size(); i++)
            {
                if (bankAccountsCollection[i]->getAccountId() == id)
                    bankAccountsCollection[i]->depositAmount(amount);
            }
        }

        else if (userChoice == 3)
        {
            int id;
            double amount;
            cout << "Enter ID and Amount: ";
            cin >> id >> amount;

            for (int i = 0; i < bankAccountsCollection.size(); i++)
            {
                if (bankAccountsCollection[i]->getAccountId() == id)
                    bankAccountsCollection[i]->withdrawAmount(amount);
            }
        }

        else if (userChoice == 4)
        {
            for (int i = 0; i < bankAccountsCollection.size(); i++)
                bankAccountsCollection[i]->displayAccountDetails();
        }

        else if (userChoice == 5)
        {
            saveAllAccounts(bankAccountsCollection);
        }

        else if (userChoice == 6)
        {
            break;
        }
    }

    for (int i = 0; i < bankAccountsCollection.size(); i++)
        delete bankAccountsCollection[i];

    bankAccountsCollection.clear();

    return 0;
}