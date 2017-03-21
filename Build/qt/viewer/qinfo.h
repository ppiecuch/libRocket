#ifndef QINFO_H
#define QINFO_H

#include <stdarg.h>

#if defined(Q_CC_MSVC) || defined(Q_CC_MSVC_NET)
inline char *f_ssprintf(const char *format, va_list args) {
   int _ss_size = _vsnprintf(0, 0, format, args);
   char *_ss_ret = (char*)alloca(_ss_size+1);
   _vsnprintf(_ss_ret, _ss_size+1, format, args);
   return _ss_ret;
}

# define qInfo(...) do { int _ss_size = _snprintf(0, 0, ##__VA_ARGS__);    \
    char *_ss_ret = (char*)alloca(_ss_size+1);                          \
    _snprintf(_ss_ret, _ss_size+1, ##__VA_ARGS__);                      \
    qDebug(_ss_ret); } while(0)

#else

# define f_ssprintf(...)                                \
    ({ int _ss_size = snprintf(0, 0, ##__VA_ARGS__);    \
    char *_ss_ret = (char*)alloca(_ss_size+1);          \
    snprintf(_ss_ret, _ss_size+1, ##__VA_ARGS__);       \
    _ss_ret; })

# define qInfo(...) do { const char *b = ({ int _ss_size = snprintf(0, 0, ##__VA_ARGS__);  \
    char *_ss_ret = (char*)alloca(_ss_size+1);                                          \
    snprintf(_ss_ret, _ss_size+1, ##__VA_ARGS__);                                       \
    _ss_ret; }); Rockete::instance->logMessage(b); } while(0)
#endif
#endif // QINFO_H
