
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Resource.h"

#define winClassName L"BINDECWinClass"

/* Convert base10 to base2, store value in output and return 0 whenever conversion succeeds. */
int convertDec2Bin(char * input, char ** output) {
    int i = 0, len = strlen(input);
    *output = malloc(VAR_LENGTH);
    char * buffer = malloc(VAR_LENGTH);

    if (*output == NULL || buffer == NULL) {
        return 1;
    }

    for(; i < len; i++) {
        if(!isdigit(input[i])) {
            return 1;
        }
    }

    int value = atoi(input), remainder;
    memset(buffer, 0, VAR_LENGTH); /* Init buffer with 0 */

    while (value != 0) { /* Perform conversion */

        remainder = value % 2;

        strcat(buffer, (remainder) ? "1" : "0");

        value /= 2;
    }

    int bufferLen = strlen(buffer), j = 0;
    memset(*output, 0, VAR_LENGTH); /* Init output with 0 */

    for(i = bufferLen - 1; i >= 0; i--) { /* Store buffer in output starting from buffer and going to start */
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
        L"BUTTON", /* Class name */
        L"Convert", /* Label */
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
        L"BUTTON",
        L"BIN2DEC",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        13,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_BIN2DEC, hInstance, NULL);

    SendMessage(hBIN2DECRadio, BM_SETCHECK, BST_CHECKED,1); //Set radio button state to checked
    SendMessage(hBIN2DECRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hDEC2BINRadio = CreateWindowEx(
        0,
        L"BUTTON",
        L"DEC2BIN",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        215,
        10,
        100,
        20,
        hwnd, (HMENU)ID_RAD_DEC2BIN, hInstance, NULL);

    SendMessage(hDEC2BINRadio, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    hInputText = CreateWindowEx(
        0,
        L"EDIT",
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
        L"EDIT",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
        10,
        70,
        270,
        25,
        hwnd, (HMENU)ID_TEX_OUTPUT, hInstance, NULL);

    SetWindowText(hOutputText, L"0"); /* Write 0 in text box for better appearance */
    SendMessage(hOutputText, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
}

void butConvertClick(HWND hwnd) {
    char input[VAR_LENGTH], * output;
    LPWSTR wideInput = (LPWSTR)GlobalAlloc(GPTR, VAR_LENGTH*sizeof(LPWSTR));
    LPWSTR wideOutput = (LPWSTR)GlobalAlloc(GPTR, VAR_LENGTH*sizeof(LPWSTR));

    GetWindowText(GetDlgItem(hwnd, ID_TEX_INPUT), wideInput, VAR_LENGTH);
    wcstombs(input, wideInput, VAR_LENGTH); /* Convert wide string to char array */

    int status = 0;

    if (SendDlgItemMessage(hwnd, ID_RAD_BIN2DEC, BM_GETCHECK, 0 , 0)) { /* Make conversion and get status */
        status = convertBin2Dec(input, &output);
    }
    else {
        status = convertDec2Bin(input, &output);
    }

    if (status == 0) { /* Conversion succeeded */
        mbstowcs(wideOutput, output, VAR_LENGTH); /* Convert char array to wide string */
    }

    SetWindowText(GetDlgItem(hwnd, ID_TEX_OUTPUT), status ? L"Internal Error." : wideOutput);

    free(output);
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
        MessageBox(NULL, L"Can't register window class!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        winClassName,
        L"BIN&DEC Converter",
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300, /* TODO: Make it a #define */
        180,
        NULL, NULL, hInstance, NULL);

    if(!hwnd) {
        MessageBox(NULL, L"Can't create window!", L"Error", MB_ICONEXCLAMATION | MB_OK);
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
