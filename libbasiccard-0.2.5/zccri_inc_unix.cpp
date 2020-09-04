#include <unistd.h>
#include <pwd.h>

#include <sys/param.h>

int ZCCriIntGetHomeDir(char *dir) {
  int uid;
  struct passwd *pw;
  char *ptr;

  if ((ptr = getenv("HOME")) != NULL) {
    strncpy(dir, ptr, MAXPATHLEN);
    return 1;
  } else {
    uid = getuid();
    pw = getpwuid(uid);
  }

  if (pw) {
    strncpy(dir, pw->pw_dir, MAXPATHLEN);
    return 1;
  }

  return 0;
}

int ZCCriIntReadLine(char *buf, int maxlen, FILE *f) {
  char *temp;
  
  if (fgets(buf, maxlen, f) == NULL) {
    return 0;
  }
  
  temp = buf + strlen(buf);
  while (*temp<0x20) {
    temp--;
  }
  temp++;
  *temp = 0;
  
  return strlen(buf);
}

/* for default reader name try
   1. environment variable (default name CRIREADER)
   2. user specific configuration file (defaults to ~/.crireader)
   3. system wide configuration file (defaults to /etc/crireader)
*/
int ZCCriIntGetDefaultReader(PZCCRIREADERNAME pName) {
  PZCCRIREADERNAME temp;
  char buffer[MAXPATHLEN+64];
  FILE *f;
  int r;
  
  temp = getenv(ENVVAR);
  if (temp != NULL) {
    strncpy(pName, temp, sizeof(ZCCRIREADERNAME));
    return 1;
  }
  
  if (ZCCriIntGetHomeDir(buffer)) {
    strcat(buffer, "/." CRI_CONFIG_FILE);
    if ((f = fopen(buffer, "r")) != NULL) {
      r = ZCCriIntReadLine(pName, sizeof(ZCCRIREADERNAME), f);
      *(((char *)pName)+r) = 0;
      fclose(f);
      if (r>0)
        return 1;
    }
  }
  
  if ((f = fopen(CRI_CONFIG_PATH "/" CRI_CONFIG_FILE, "r")) != NULL) {
    r = ZCCriIntReadLine(pName, sizeof(ZCCRIREADERNAME), f);
    *(((char *)pName)+r) = 0;
    fclose(f);
    if (r>0)
      return 1;
  }
  
  return 0;
}

/* permanently save default reader name
   in user specific configuration file
*/
int ZCCriIntPermSaveDefaultReader(PZCCRIREADERNAME pName) {
  char buffer[MAXPATHLEN+64];
  FILE *f;
  int r;
  
  if (ZCCriIntGetHomeDir(buffer)) {
    strcat(buffer, "/." CRI_CONFIG_FILE);
    if ((f = fopen(buffer, "w")) != NULL) {
      fprintf(f, "%s\n", pName);
      fflush(f);
      r = ferror(f);
      fclose(f);
      return (!r);
    }
  }
  
  return 0;
}
