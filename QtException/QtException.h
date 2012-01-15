/**
  @file     QtException.h
  @author   Andreas Poesch   (andreas.poesch@imr.uni-hannover.de)
  @since    2010 / 03 / 01
  @brief    excption handling and macros

  @todo all those if(level)...statements should be replaced by some "#if"-precompiler statements so that the if-clause does not appear in the compiled code

  (c) 2010, Copyright Andreas Poesch IMR Hannover

  $Id: QtException.h 72 2011-05-07 11:13:42Z mechaot $
  
  Usage:
  
  ASSERT(x)				: if condition is false, display error message and continue
  ASSERT_RETURN(x)		: if condition is false, display error message and return from current function
  ASSERT_DO(x, code)    : if condition is false, display error message and perform code (e.g. "return somevalue;")
  THROW(msg)			: throw an exception with message msg
  ASSERT_THROW(x)		: if condition is false, log error message and throw an appropriate exception

  DEBUG(level, msg)		: if level is lower or equal to {DEBUG_LEVEL_STD, DEBUG_LEVEL_LOG, DEBUG_LEVEL_GUI} queue a debug message there
  CONSOLE(level, msg)   : if level is lower or equal to {DEBUG_LEVEL_STD, DEBUG_LEVEL_LOG} queue a debug message there

  DebugLevel Conventions:

  1: Fatal Messages
  2-9: Important messages
  10-99: Maybe-Interesting Messages
  100+: Exhaustive Messages


**/


//a negative value disables logging

#ifndef QTEXCEPTION_H
#define QTEXCEPTION_H

#define EXCEPTION_USE_GUI 0

#include <QtCore>
#include <QObject>

#if EXCEPTION_USE_GUI
    #include <QMessageBox>
    #include <QtGui>
#endif

#include <QString>
#include <QFile>
#include <exception>



class QtException;
class QDebugger;

extern QDebugger   *gDebug;



#define DEBUGGER_STD_LEVEL      12
#define DEBUGGER_GUI_LEVEL      3
#define DEBUGGER_STD_FILENAME   "debug.txt"
#define DEBUGGER_MULTITHREADING 1

#define DEBUGGER_PARANOID_FLUSH 1           //flush file buffer after every line
#define DEBUGGER_PRINT_SOURCE_POS 1         //print source position of any message (file, line)
#define DEBUGGER_TIMESTAMP       1
//#define DEBUGGER_HTML           1           //make html output

//#pragma once      //uncomment for

/*************************  PRETTY FUNCTION NAMING   *************************/
#ifndef __PRETTY_FUNCTION__
    #ifdef _MSC_VER //visual studio has another style
        #define __PRETTY_FUNCTION__ __FUNCSIG__         // alternative __FUNCDNAME__
    #else
        #ifndef __GNUC__    //neither visual studio nor gcc
            #define __PRETTY_FUNCTION__ __FUNCTION__
        #endif
    #endif
#endif

#if DEBUGGER_PRINT_SOURCE_POS
    #define SOURCEPOS          QString(" %1 in \"%2\" Line: #%3").arg(__PRETTY_FUNCTION__).arg(__FILE__).arg(__LINE__)
#else
    #define SOURCEPOS          QString()
#endif

#define DEBUGSTRING(l,msg)   QString("Debug (level = %1) in %2: '<b>%3</b>'").arg(l).arg(SOURCEPOS).arg(msg)
#define ERRORSTRING(msg)     QString("Error in %1 '<b>%2</b>'").arg(SOURCEPOS).arg(msg)


#define _MESSAGE(msg) { gDebug->outConsole(QString("Assert Failed: '%1'").arg(ERRORSTRING(msg))); gDebug->outFile(QString("Assert Failed: '%1'").arg(ERRORSTRING(msg))); }
#define _MESSAGE_X(msg) { gDebug->debug(QString("Assert Failed: '%1'").arg(ERRORSTRING(msg))); }

#if EXCEPTION_USE_GUI
    #define MESSAGE(x) _MESSAGE_X(x)
#else
    #define MESSAGE(x) _MESSAGE(x)
#endif

#define ASSERT(x)                 { if (!(x)) { MESSAGE(#x); } }
#define ASSERT_RETURN(x)          { if (!(x)) { MESSAGE(#x); return; } }
#define ASSERT_RETURNVAL(x, RVAL) { if (!(x)) { MESSAGE(#x); return RVAL; } }
#define ASSERT_DO(x, code) 	      { if (!(x)) { MESSAGE(#x); code; } }
#define ASSERT_PASS(x)            { if (!(x)) { MESSAGE(#x); } }
#define THROW(msg)				  { MESSAGE(#msg); throw QtException(QString("Assert failed!<br><br><b>%1</b>").arg(ERRORSTRING(#msg))); }
/** throw en exception if cond is not met **/
#define ASSERT_THROW(cond) 		  {if(!(cond)) { THROW(cond); } }


//debug to all streams according to levels
#define DEBUG(level,x) gDebug->debug(QString("%1 in %2 (level %3)").arg(x).arg(SOURCEPOS).arg(level), level)
//debug message only to console and file, according to levels
#define CONSOLE(level,x)  { if (level <=  gDebug->m_iLevelFile) { gDebug->outFile(QString("%1 in %2 (level %3)").arg(x).arg(SOURCEPOS).arg(level)); } if (level <= gDebug->m_iLevelConsole) { gDebug->outConsole(QString("%1 in %2 (level %3)").arg(x).arg(SOURCEPOS).arg(level)); } }

//by any means output this message
#define DBG(msg) DEBUG(-1, msg)

/** Definately throw an exception with message s and display a message box  **/
#define EX_THROW(s) { QtException e(QString("Exception thrown: %1").arg(ERRORSTRING(s))); throw e;}


/**
  @class QtException        exception class

  The throw()-declaration makes the compiler know that the exception itself may never cause an exeption by itself

  This class has a lot of statics to
  **/
class QtException : public std::exception  {

private:


private:
    QString m_msg;

public:
    QtException(const QString &msg  = QString() ) throw();
    virtual ~QtException() throw() ;

    virtual QString toString()  throw();
    virtual const char* what() const throw();
};


void DEBUG_INIT(QString fileName = DEBUGGER_STD_FILENAME);
void DEBUG_CLOSE();

/**
  @class    QDebugger       debugging object
  **/
class QDebugger : public QObject
{
Q_OBJECT
public:
    QFile   m_file;
    int     m_iLevelConsole;
    int     m_iLevelGui;
    int     m_iLevelFile;
    int     m_iCounterFile;
    int     m_iCounterConsole;

#ifdef DEBUGGER_MULTITHREADING
    QMutex  m_mutex;
#endif

public:
    QDebugger(const QString &fileName = QString());
    virtual ~QDebugger();

    void setFileName(const QString &fileName);
    void closeFile();

public :
    void outFile(const QString &str);
    void outGui(const QString &str);
    void outConsole(const QString &str);

    void debug(const QString &msg, int level = -1);

signals:
    void console(const QString &msg);
};

#endif // QTEXCEPTION_H
