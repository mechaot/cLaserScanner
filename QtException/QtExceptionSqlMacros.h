#ifndef QTEXCEPTIONSQLMACROS_H
#define QTEXCEPTIONSQLMACROS_H

#include "main.h"
#include "QtException.h"

#define SQL_ERROR_LOGLEVEL 2

#define SQL_CHECK_DO(q, s, code) \
if(q .lastError().isValid()) { \
    QString errmsg = QString("<html><b>Datenbankfehler!</b><br>SQL-Anfrage:<pre>%1</pre><br>SQL-Fehlermeldung:<br><pre>%2</pre><br><p>%3</p></html>").arg(q .lastQuery()).arg(q .lastError().text()).arg( s); \
    DEBUG(SQL_ERROR_LOGLEVEL, errmsg); \
    code ; \
}

#define SQL_CHECK_RETURN(q, s) \
    SQL_CHECK_DO(q, s, return)

#define SQL_CHECK(q, s) \
    SQL_CHECK_DO(q, s, ;)

#endif // QTEXCEPTIONSQLMACROS_H
