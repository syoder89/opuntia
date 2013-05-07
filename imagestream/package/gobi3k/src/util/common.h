#ifndef COMMON_H
#define COMMON_H
const char *gobi_strerr(ULONG gobi_errno);
const char *pin_status_str(ULONG pin_status);
void fail(ULONG err, const char *desc);
void connect_unlock();
#endif
