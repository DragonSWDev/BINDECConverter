
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Resource.h"

#define winClassName TEXT("BINDECWinClass")

/* Convert base10 to base2, store value in output and return 0 whenever conversion succeeds. */
int convertDec2Bin(char * input, char ** output) {
    int i = 0, j = 0, len = strlen(input), value, remainder, bufferlen;
    char * buffer = malloc(VAR_LENGTH);

    *output = malloc(VAR_LENGTH);

    if (*output == NULL || buffer == NULL) {
        return 1;
    }

    for(; i < len; i++) {
        if(!isdigit(input[i])) {
            return 1;
        }
    }

    value = atoi(input);
    memset(buffer, 0, VAR_LENGTH); /* Init buffer with 0 */

    while (value != 0) { /* Perform conversion */

        remainder = value % 2;

        strcat(buffer, (remainder) ? "1" : "0");

        value /= 2;
    }

    bufferlen = strlen(buffer);

    memset(*output, 0, VAR_LENGTH); /* Init output with 0 */

    for(i = bufferlen - 1; i >= 0; i--) { /* Store buffer in output starting from buffer and going to start */
        memset(*output + j, buffer[i], 1);
        j++;
    }

    free(buffer);

    return 0;
}

/* Convert base10 to base2, store value in output and return 0 whenever conversion succeeds. */
int convertBin2Dec(char * input, char ** output) {
    int value = 0, power = strlen(input) - 1, i = 0, len = strlen(input);
    *output = malloc(VAR_LENGTH);

    if (*output == NULL) {
        return 1;
    }

    for(; i < len; i++)
        if(input[i] != '0' && input[i] != '1')
            return 1;
            

    for(i = 0; i < len; i++, power--) {
        if(input[i] == '1') {
            value += pow(2, power);
        }
    }

    sprintf(*output, "%d", value);

    return 0;
}

void createUI(HWND hwnd) {
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
    HWND hConvertButton, hBIN2DECRadio, hDEC2BINRadio, hInputText, hOutputText;

    hConvertButton = CreateWindowEx(
        0,
        TEXT("BUTTON"), /* Class name */
        TEXT("Convert"), /* Label */
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, /* Properties */
        10, /* X */
        100, /* Y */
        270, /* Width */
        40, /* Height */
        hwnd, (HMENU)ID_BUT_CONVERT, hInstance, NULL); /* Parent window, ID, parent instance */

    /* Set font to default system font */
    SendMessage(hConvertButton, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hBIN2DECRadio = CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("BIN2DEC"),
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        13,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_BIN2DEC, hInstance, NULL);

    SendMessage(hBIN2DECRadio, BM_SETCHECK, BST_CHECKED,1); /* Set radio button state to checked */
    SendMessage(hBIN2DECRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hDEC2BINRadio = CreateWindowEx(
        0,
        TEXT("BUTTON"),
        TEXT("DEC2BIN"),
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        215,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_DEC2BIN, hInstance, NULL);

    SendMessage(hDEC2BINRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hInputText = CreateWindowEx(
        0,
        TEXT("EDIT"),
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        10,
        40,
        270,
        25,
        hwnd, (HMENU)ID_TEX_INPUT, hInstance, NULL);

    SendMessage(hInputText, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hOutputText = CreateWindowEx(
        0,
        TEXT("EDIT"),
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
        10,
        70,
        270,
        25,
        hwnd, (HMENU)ID_TEX_OUTPUT, hInstance, NULL);

    SetWindowText(hOutputText, TEXT("0")); /* Write 0 in text box for better appearance */
    SendMessage(hOutputText, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
}

void butConvertClick(HWND hwnd) {
    char input[VAR_LENGTH], * output;
    int status;

    LPTSTR txtInput = (LPTSTR)GlobalAlloc(GPTR, VAR_LENGTH*sizeof(LPTSTR));
    LPTSTR txtOutput = (LPTSTR)GlobalAlloc(GPTR, VAR_LENGTH*sizeof(LPTSTR));

    GetWindowText(GetDlgItem(hwnd, ID_TEX_INPUT), txtInput, VAR_LENGTH);

    #if defined(UNICODE) || defined(_UNICODE) /* Unicode */
        wcstombs(input, txtInput, VAR_LENGTH); /* Convert wide string to char array */
    #else /* ANSI */
        strcpy(input, (char*)(txtInput)); /* Copy value from txtInput to input - no need to convert */
    #endif

    status = 0;

    if (SendDlgItemMessage(hwnd, ID_RAD_BIN2DEC, BM_GETCHECK, 0 , 0)) { /* Make conversion and get status */
        status = convertBin2Dec(input, &output);
    }
    else {
        status = convertDec2Bin(input, &output);
    }

    if (status == 0) { /* Conversion succeeded */
        #if defined(UNICODE) || defined(_UNICODE) /* Unicode */
            mbstowcs(txtOutput, output, VAR_LENGTH); /* Convert char array to wide string */
        #else /* ANSI */
            strcpy((char*)(txtOutput), output); 
        #endif
    }

    SetWindowText(GetDlgItem(hwnd, ID_TEX_OUTPUT), status ? TEXT("Internal Error.") : txtOutput);

    free(output);
    GlobalFree(txtInput);
    GlobalFree(txtOutput);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
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

        case WM_CTLCOLORSTATIC:
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Can't register window class!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        winClassName,
        TEXT("BIN&DEC Converter"),
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300, /* TODO: Make it a #define */
        180,
        NULL, NULL, hInstance, NULL);

    if(!hwnd) {
        MessageBox(NULL, TEXT("Can't create window!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
