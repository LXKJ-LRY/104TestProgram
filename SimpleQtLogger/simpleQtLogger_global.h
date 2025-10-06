#pragma once

#include <qglobal.h>

#if defined(SIMPLEQTLOGGER_LIBRARY)
  #define SIMPLEQTLOGGER_EXPORT Q_DECL_EXPORT
#else
  #define SIMPLEQTLOGGER_EXPORT Q_DECL_IMPORT
#endif
