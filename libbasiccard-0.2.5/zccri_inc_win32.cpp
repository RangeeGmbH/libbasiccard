// windows DLL entry point
HINSTANCE hinstance = 0;
extern "C" {
BOOL WINAPI DllMain (HANDLE hDll, DWORD dwReason, LPVOID lpReserved) {
  hinstance = (HINSTANCE)hDll;
  return 1;
}
}

#define ID_OK 1
#define ID_CANCEL 2
#define ID_LISTBOX 1000
unsigned int ZCCriIntReaderCount();

// windows reader selection dialog
BOOL CALLBACK ZCCriIntWinDialogProc(HWND hwndDlg, UINT uMsg,	WPARAM wParam, LPARAM lParam) {
  static PZCCRIREADERNAME pName;
  static HWND listbox;
  ZCCRIREADERNAME temp;
  unsigned int i;
  LRESULT r;
  
  switch (uMsg) {
    case WM_INITDIALOG:
      pName = (PZCCRIREADERNAME)lParam;
      listbox = GetDlgItem(hwndDlg, ID_LISTBOX);
      for (i=0; i<ZCCriIntReaderCount(); i++) {
        ZCCriGetReaderName(i, temp);
        SendMessage(listbox, LB_ADDSTRING, 0, (LPARAM)temp);
      }
      return 1;
      break;
      
    case WM_COMMAND:
      switch (wParam & 0xffff) {
        case ID_CANCEL:
          EndDialog(hwndDlg, ID_CANCEL);
          break;
        case ID_OK:
          r = SendMessage(listbox, LB_GETCURSEL, 0, 0);
          if (r == LB_ERR) {
            // no selection
            EndDialog(hwndDlg, ID_CANCEL);
            break;
          }
          i = r;
          r = SendMessage(listbox, LB_GETTEXTLEN, i, 0);
          if (r >= (signed int)sizeof(ZCCRIREADERNAME)) {
            // text too long, this should not happen
            EndDialog(hwndDlg, ID_CANCEL);
            break;
          }
          SendMessage(listbox, LB_GETTEXT, i, (LPARAM)pName);
          EndDialog(hwndDlg, ID_OK);
          break;
      }
      return 1;
      break;
  }
  
  return 0;
}

ZCCRIRET ZCCriIntGuiSelectReaderDialog(PZCCRIREADERNAME pName) {
  if (DialogBoxParam(hinstance, MAKEINTRESOURCE(101), 0, &ZCCriIntWinDialogProc, (LPARAM)pName) == ID_OK) {
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_CANCEL;
  }
}

// read default reader from the registry if not overridden by environment variable
int ZCCriIntGetDefaultReader(PZCCRIREADERNAME pName) {
  PZCCRIREADERNAME temp;
  HKEY subkey;
  DWORD len = sizeof(ZCCRIREADERNAME);
  LONG res;
  
  temp = getenv(ENVVAR);
  if (temp != NULL) {
    strncpy(pName, temp, sizeof(ZCCRIREADERNAME));
    return 1;
  }
  
  if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\ZeitControl\\ZCCRI\\Default", &subkey) != ERROR_SUCCESS) {
    return 0;
  }
  res = RegQueryValueEx(subkey, "Default", NULL, NULL, (LPBYTE)pName, &len);
  RegCloseKey(subkey);
  
  return (res==ERROR_SUCCESS)?1:0;
}

int ZCCriIntPermSaveDefaultReader(PZCCRIREADERNAME pName) {
  HKEY subkey;
  LONG res;

  if (RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\ZeitControl\\ZCCRI\\Default", &subkey) != ERROR_SUCCESS) {
    return 0;
  }
  
  res = RegSetValueEx(subkey, "Default", 0, REG_SZ, (CONST BYTE *)pName, strlen(pName));
  RegCloseKey(subkey);
  
  return (res==ERROR_SUCCESS)?1:0;  
}
