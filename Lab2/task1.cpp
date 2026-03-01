#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class TableColumn
{
public:
    string columnName;
    string columnDataType;
    unsigned int columnConstraints;

    TableColumn(string name, string type, unsigned int constraintsValue)
    {
        columnName = name;
        columnDataType = type;
        columnConstraints = constraintsValue;
    }
};

class TableRow
{
public:
    vector<string> columnValues;
};

class DatabaseTable
{
private:
    string tableIdentifier;
    vector<TableColumn> tableColumns;
    vector<TableRow*> tableRows;

public:
    DatabaseTable(string name)
    {
        tableIdentifier = name;
    }

    ~DatabaseTable()
    {
        for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
        {
            delete tableRows[rowIndex];
        }
        tableRows.clear();
    }

    string getTableName()
    {
        return tableIdentifier;
    }

    void insertRecord(vector<string> inputColumnValues)
    {
        if (!validateRecord(inputColumnValues))
            return;

        TableRow* newTableRow = new TableRow();
        newTableRow->columnValues = inputColumnValues;
        tableRows.push_back(newTableRow);

        cout << "Record inserted successfully.\n";
    }

    void displayAllRecords()
    {
        for (int columnIndex = 0; columnIndex < tableColumns.size(); columnIndex++)
        {
            cout << tableColumns[columnIndex].columnName << "\t";
        }
        cout << endl;

        for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
        {
            for (int valueIndex = 0; valueIndex < tableRows[rowIndex]->columnValues.size(); valueIndex++)
            {
                cout << tableRows[rowIndex]->columnValues[valueIndex] << "\t";
            }
            cout << endl;
        }
    }

    void storeTableToFile(string filePath)
    {
        ofstream outputFile(filePath);

        outputFile << "TABLE " << tableIdentifier << endl;

        for (int columnIndex = 0; columnIndex < tableColumns.size(); columnIndex++)
        {
            outputFile << tableColumns[columnIndex].columnName << " "
                       << tableColumns[columnIndex].columnDataType << " "
                       << tableColumns[columnIndex].columnConstraints << endl;
        }

        outputFile << "DATA" << endl;

        for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
        {
            for (int valueIndex = 0; valueIndex < tableRows[rowIndex]->columnValues.size(); valueIndex++)
            {
                outputFile << tableRows[rowIndex]->columnValues[valueIndex] << " ";
            }
            outputFile << endl;
        }

        outputFile.close();
        cout << "Saved to file successfully.\n";
    }

    void loadTableFromFile(string filePath)
    {
        ifstream inputFile(filePath);

        if (!inputFile)
        {
            cout << "File not found.\n";
            return;
        }

        tableColumns.clear();

        string readWord;
        inputFile >> readWord;
        inputFile >> tableIdentifier;

        string columnName;
        string columnType;
        unsigned int constraintFlags;

        while (inputFile >> readWord)
        {
            if (readWord == "DATA")
                break;

            columnName = readWord;
            inputFile >> columnType;
            inputFile >> constraintFlags;

            tableColumns.push_back(TableColumn(columnName, columnType, constraintFlags));
        }

        for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
            delete tableRows[rowIndex];

        tableRows.clear();

        while (!inputFile.eof())
        {
            TableRow* loadedRow = new TableRow();

            for (int columnIndex = 0; columnIndex < tableColumns.size(); columnIndex++)
            {
                string columnValue;
                inputFile >> columnValue;
                if (inputFile.fail())
                    break;
                loadedRow->columnValues.push_back(columnValue);
            }

            if (loadedRow->columnValues.size() == tableColumns.size())
                tableRows.push_back(loadedRow);
            else
                delete loadedRow;
        }

        inputFile.close();
        cout << "Loaded from file successfully.\n";
    }

    void addNewColumn(string name, string type, unsigned int constraintValue)
    {
        tableColumns.push_back(TableColumn(name, type, constraintValue));
    }

    bool validateRecord(vector<string> inputColumnValues)
    {
        for (int columnIndex = 0; columnIndex < tableColumns.size(); columnIndex++)
        {
            unsigned int constraintFlags = tableColumns[columnIndex].columnConstraints;

            if ((constraintFlags & 2) && inputColumnValues[columnIndex] == "")
            {
                cout << "Not Null constraint violated.\n";
                return false;
            }

            if (constraintFlags & 4)
            {
                for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
                {
                    if (tableRows[rowIndex]->columnValues[columnIndex] == inputColumnValues[columnIndex])
                    {
                        cout << "Unique constraint violated.\n";
                        return false;
                    }
                }
            }

            if (constraintFlags & 1)
            {
                for (int rowIndex = 0; rowIndex < tableRows.size(); rowIndex++)
                {
                    if (tableRows[rowIndex]->columnValues[columnIndex] == inputColumnValues[columnIndex])
                    {
                        cout << "Primary Key duplicate.\n";
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

int main()
{
    char commandBuffer[256];
    DatabaseTable* activeTable = NULL;
    int totalColumnCount = 0;

    while (true)
    {
        cout << "\nEnter Command: ";
        cin.getline(commandBuffer, 256);

        string userCommand = commandBuffer;

        if (userCommand.find("CREATE TABLE") != string::npos)
        {
            string newTableName;
            cout << "Enter table name: ";
            cin >> newTableName;

            activeTable = new DatabaseTable(newTableName);

            cout << "Enter number of columns: ";
            cin >> totalColumnCount;

            for (int columnIndex = 0; columnIndex < totalColumnCount; columnIndex++)
            {
                string columnName;
                string columnType;
                unsigned int constraintValue;

                cout << "Column name: ";
                cin >> columnName;

                cout << "Type (int/string): ";
                cin >> columnType;

                cout << "Constraints (1=PK,2=NN,4=Unique, combine using OR): ";
                cin >> constraintValue;

                activeTable->addNewColumn(columnName, columnType, constraintValue);
            }

            cout << "Table created successfully.\n";
            cin.ignore();
        }

        else if (userCommand.find("INSERT") != string::npos)
        {
            if (!activeTable)
            {
                cout << "No table created.\n";
                continue;
            }

            vector<string> recordValues;
            string inputValue;

            for (int columnIndex = 0; columnIndex < totalColumnCount; columnIndex++)
            {
                cin >> inputValue;
                recordValues.push_back(inputValue);
            }

            activeTable->insertRecord(recordValues);
            cin.ignore();
        }

        else if (userCommand.find("SELECT") != string::npos)
        {
            if (activeTable)
                activeTable->displayAllRecords();
        }

        else if (userCommand.find("SAVE") != string::npos)
        {
            if (activeTable)
                activeTable->storeTableToFile("database.txt");
        }

        else if (userCommand.find("LOAD") != string::npos)
        {
            if (!activeTable)
                activeTable = new DatabaseTable("temporary");

            activeTable->loadTableFromFile("database.txt");
        }

        else if (userCommand == "EXIT")
        {
            delete activeTable;
            break;
        }
    }

    return 0;
}