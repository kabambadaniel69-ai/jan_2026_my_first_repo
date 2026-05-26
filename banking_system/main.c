#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 100
#define MAX_ACCOUNT_NUMBER 20
#define MAX_NAME_LENGTH 50
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 500

// Structure for account
typedef struct {
    char accountNumber[MAX_ACCOUNT_NUMBER];
    char accountHolder[MAX_NAME_LENGTH];
    double balance;
    char createdDate[20];
} Account;

// Global variables
Account accounts[MAX_ACCOUNTS];
int accountCount = 0;
HWND hMainWindow;
HWND hOutput;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateAccount(const char *accountNumber, const char *accountHolder, double initialBalance);
void Deposit(const char *accountNumber, double amount);
void Withdraw(const char *accountNumber, double amount);
double CheckBalance(const char *accountNumber);
void DisplayMessage(const char *message);
void ClearOutput();
int FindAccount(const char *accountNumber);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "BankingSystemClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    hMainWindow = CreateWindowEx(
        0,
        CLASS_NAME,
        "Simple Banking System",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (hMainWindow == NULL) return 0;
    
    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hAcctNum, hName, hAmount, hResult;
    
    switch (msg) {
        case WM_CREATE: {
            // Title
            CreateWindowEx(0, "STATIC", "BANKING SYSTEM",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                10, 10, 560, 30, hwnd, NULL, NULL, NULL);
            
            // Account Number input
            CreateWindowEx(0, "STATIC", "Account Number:",
                WS_VISIBLE | WS_CHILD, 20, 50, 120, 20, hwnd, NULL, NULL, NULL);
            hAcctNum = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_VISIBLE | WS_CHILD | ES_LEFT,
                150, 50, 400, 25, hwnd, NULL, NULL, NULL);
            
            // Account Holder Name
            CreateWindowEx(0, "STATIC", "Account Holder:",
                WS_VISIBLE | WS_CHILD, 20, 85, 120, 20, hwnd, NULL, NULL, NULL);
            hName = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_VISIBLE | WS_CHILD | ES_LEFT,
                150, 85, 400, 25, hwnd, NULL, NULL, NULL);
            
            // Amount input
            CreateWindowEx(0, "STATIC", "Amount:",
                WS_VISIBLE | WS_CHILD, 20, 120, 120, 20, hwnd, NULL, NULL, NULL);
            hAmount = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_VISIBLE | WS_CHILD | ES_LEFT,
                150, 120, 400, 25, hwnd, NULL, NULL, NULL);
            
            // Buttons
            CreateWindowEx(0, "BUTTON", "1. Create Account",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 160, 110, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "2. Deposit",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                140, 160, 110, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "3. Withdraw",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                260, 160, 110, 30, hwnd, (HMENU)3, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "4. Check Balance",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                380, 160, 110, 30, hwnd, (HMENU)4, NULL, NULL);
            CreateWindowEx(0, "BUTTON", "5. Exit",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                260, 200, 80, 30, hwnd, (HMENU)5, NULL, NULL);
            
            // Output display
            hOutput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
                20, 240, 560, 230, hwnd, NULL, NULL, NULL);
            
            DisplayMessage("Welcome to Simple Banking System!\nSelect an option to begin.");
            break;
        }
        
        case WM_COMMAND: {
            char acctNum[MAX_ACCOUNT_NUMBER];
            char name[MAX_NAME_LENGTH];
            char amountStr[20];
            double amount;
            
            GetWindowText(hAcctNum, acctNum, sizeof(acctNum));
            GetWindowText(hName, name, sizeof(name));
            GetWindowText(hAmount, amountStr, sizeof(amountStr));
            
            switch (LOWORD(wParam)) {
                case 1: { // Create Account
                    if (strlen(acctNum) == 0 || strlen(name) == 0 || strlen(amountStr) == 0) {
                        DisplayMessage("ERROR: Please fill all fields (Account Number, Name, Initial Balance)");
                    } else if (FindAccount(acctNum) != -1) {
                        DisplayMessage("ERROR: Account already exists!");
                    } else {
                        amount = atof(amountStr);
                        if (amount < 0) {
                            DisplayMessage("ERROR: Initial balance cannot be negative!");
                        } else {
                            CreateAccount(acctNum, name, amount);
                            char msg[200];
                            sprintf_s(msg, sizeof(msg), "SUCCESS: Account %s created for %s\nInitial Balance: $%.2f", acctNum, name, amount);
                            DisplayMessage(msg);
                            SetWindowText(hAcctNum, "");
                            SetWindowText(hName, "");
                            SetWindowText(hAmount, "");
                        }
                    }
                    break;
                }
                case 2: { // Deposit
                    if (strlen(acctNum) == 0 || strlen(amountStr) == 0) {
                        DisplayMessage("ERROR: Please enter Account Number and Amount");
                    } else {
                        amount = atof(amountStr);
                        if (amount <= 0) {
                            DisplayMessage("ERROR: Deposit amount must be positive!");
                        } else if (FindAccount(acctNum) == -1) {
                            DisplayMessage("ERROR: Account not found!");
                        } else {
                            Deposit(acctNum, amount);
                            char msg[200];
                            sprintf_s(msg, sizeof(msg), "SUCCESS: Deposited $%.2f\nNew Balance: $%.2f", amount, accounts[FindAccount(acctNum)].balance);
                            DisplayMessage(msg);
                            SetWindowText(hAmount, "");
                        }
                    }
                    break;
                }
                case 3: { // Withdraw
                    if (strlen(acctNum) == 0 || strlen(amountStr) == 0) {
                        DisplayMessage("ERROR: Please enter Account Number and Amount");
                    } else {
                        amount = atof(amountStr);
                        if (amount <= 0) {
                            DisplayMessage("ERROR: Withdrawal amount must be positive!");
                        } else if (FindAccount(acctNum) == -1) {
                            DisplayMessage("ERROR: Account not found!");
                        } else if (accounts[FindAccount(acctNum)].balance < amount) {
                            DisplayMessage("ERROR: Insufficient balance!");
                        } else {
                            Withdraw(acctNum, amount);
                            char msg[200];
                            sprintf_s(msg, sizeof(msg), "SUCCESS: Withdrew $%.2f\nNew Balance: $%.2f", amount, accounts[FindAccount(acctNum)].balance);
                            DisplayMessage(msg);
                            SetWindowText(hAmount, "");
                        }
                    }
                    break;
                }
                case 4: { // Check Balance
                    if (strlen(acctNum) == 0) {
                        DisplayMessage("ERROR: Please enter Account Number");
                    } else if (FindAccount(acctNum) == -1) {
                        DisplayMessage("ERROR: Account not found!");
                    } else {
                        int idx = FindAccount(acctNum);
                        char msg[300];
                        sprintf_s(msg, sizeof(msg), 
                            "ACCOUNT DETAILS\n"
                            "Account Number: %s\n"
                            "Account Holder: %s\n"
                            "Current Balance: $%.2f\n"
                            "Created: %s",
                            accounts[idx].accountNumber,
                            accounts[idx].accountHolder,
                            accounts[idx].balance,
                            accounts[idx].createdDate);
                        DisplayMessage(msg);
                    }
                    break;
                }
                case 5: { // Exit
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                }
            }
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    return 0;
}

void CreateAccount(const char *accountNumber, const char *accountHolder, double initialBalance) {
    if (accountCount < MAX_ACCOUNTS) {
        strcpy_s(accounts[accountCount].accountNumber, MAX_ACCOUNT_NUMBER, accountNumber);
        strcpy_s(accounts[accountCount].accountHolder, MAX_NAME_LENGTH, accountHolder);
        accounts[accountCount].balance = initialBalance;
        
        time_t now = time(NULL);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(accounts[accountCount].createdDate, 20, "%Y-%m-%d %H:%M", &timeinfo);
        
        accountCount++;
    }
}

void Deposit(const char *accountNumber, double amount) {
    int idx = FindAccount(accountNumber);
    if (idx != -1) {
        accounts[idx].balance += amount;
    }
}

void Withdraw(const char *accountNumber, double amount) {
    int idx = FindAccount(accountNumber);
    if (idx != -1 && accounts[idx].balance >= amount) {
        accounts[idx].balance -= amount;
    }
}

double CheckBalance(const char *accountNumber) {
    int idx = FindAccount(accountNumber);
    if (idx != -1) {
        return accounts[idx].balance;
    }
    return -1;
}

int FindAccount(const char *accountNumber) {
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].accountNumber, accountNumber) == 0) {
            return i;
        }
    }
    return -1;
}

void DisplayMessage(const char *message) {
    SetWindowText(hOutput, message);
}
