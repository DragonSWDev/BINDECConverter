//Windows headers
#include <windows.h>
#include <commctrl.h>

//Standard C headers
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Resource.h"

const char winClassName[] = "BINDECWinClass";

bool convertDec2Bin(char input[], char **output) //Convert DEC to BIN, save value in output and return result
{
    *output = malloc(VAR_LENGHT);

    for(int i = 0; i < strlen(input); i++)
        if(input[i] < '0' || input[i] > '9')
            return false;

    int value = atoi(input);
    itoa(value, *output, 2);
    
    return true;
}

bool convertBin2Dec(char input[], char **output) ////Convert BIN to DEC, save value in output and return result
{
    *output = malloc(VAR_LENGHT);
    int value = 0;

    for(int i = 0; i < strlen(input); i++)
        if(input[i] < '0' || input[i] > '1')
            return false;

    int power = strlen(input) - 1;

    for(int i = 0; i < strlen(input); i++)
    {
        if(input[i] == '1')
            value += pow(2, power);

        power--;
    }

    itoa(value, *output, 10);

    return true;
}

void createUI(HWND hwnd) //Create controls like buttons and text boxes
{
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    HWND hConvertButton = CreateWindowEx(
        0,
        "BUTTON", //Class name
        "Convert", //Label
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, //Properties
        10, //X
        100, //Y
        270, //Width
        40, //Height
        hwnd, (HMENU)ID_BUT_CONVERT, hInstance, NULL); //Parent window, ID, parent instance

    //Set font to default system font
    SendMessage(hConvertButton, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    HWND hBIN2DECRadio = CreateWindowEx(
        0,
        "BUTTON",
        "BIN2DEC",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        13,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_BIN2DEC, hInstance, NULL);

    SendMessage(hBIN2DECRadio, BM_SETCHECK, BST_CHECKED,1); //Set radio button state to checked
    SendMessage(hBIN2DECRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    HWND hDEC2BINRadio = CreateWindowEx(
        0,
        "BUTTON",
        "DEC2BIN",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        215,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_DEC2BIN, hInstance, NULL);

    SendMessage(hDEC2BINRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    HWND hInputText = CreateWindowEx(
        0,
        "EDIT",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        10,
        40,
        270,
        25,
        hwnd, (HMENU)ID_TEX_INPUT, hInstance, NULL);

    SendMessage(hInputText, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    HWND hOutputText = CreateWindowEx(
        0,
        "EDIT",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
        10,
        70,
        270,
        25,
        hwnd, (HMENU)ID_TEX_OUTPUT, hInstance, NULL);

    SetWindowText(hOutputText, "0"); //Write 0 in text box for better appearance
    SendMessage(hOutputText, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
}

void butConvertClick(HWND hwnd)
{
    char input[VAR_LENGHT];
    char *output;
    bool result;
                
    GetWindowText(GetDlgItem(hwnd, ID_TEX_INPUT), input, VAR_LENGHT);

    //Check which radio button is checked
    if(SendDlgItemMessage(hwnd, ID_RAD_BIN2DEC, BM_GETCHECK, 0 , 0))
        result = convertBin2Dec(input, &output);
    else
        result = convertDec2Bin(input, &output);

    if(result)
        SetWindowText(GetDlgItem(hwnd, ID_TEX_OUTPUT), output);
    else
        SetWindowText(GetDlgItem(hwnd, ID_TEX_OUTPUT), "Input error");
                
    free(output); //Free array allocated by function        
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            createUI(hwnd);
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            if(LOWORD(wParam) == ID_BUT_CONVERT)
                butConvertClick(hwnd);

            break;

        case WM_CTLCOLORSTATIC: //Without it radio button has gray background
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
    INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = winClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Can't register window class!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        winClassName,
        "BIN&DEC Converter",
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300,
        180,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Can't create window!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
