/**
  @file     QtException.cpp
  @author   Andreas Poesch   (andreas.poesch@imr.uni-hannover.de)
  @since    2010 / 03 / 01
  @brief    excption handling and macros

  (c) 2010, Copyright Andreas Poesch IMR Hannover

  $Id: QtException.cpp 72 2011-05-07 11:13:42Z mechaot $
**/

#include "QtException.h"

#include <QFile>
#include <stdio.h>



QDebugger   *gDebug = NULL;

static const QString empty_msg("unknown exception");

/**
  @brief    initializing constructor
  @param    msg message to display/output for debugging
**/
QtException::QtException(const QString &msg /* = QString() */) throw() : std::exception()
{

   if (msg.isEmpty())
       m_msg = empty_msg;
   else
       m_msg = msg;

   if (gDebug) gDebug->debug(msg,-1);
}

/**
  @brief    destructor
**/
QtException::~QtException() throw()
{
}

/**
  @brief    convert exception to a string
  @return   string
**/
QString QtException::toString() throw()
{
    return m_msg;
}

/**
  @brief    std::exception like output function
  @return   string describing the exception

  @note possible memory leak
  **/
const char* QtException::what() const throw()
{
    //need to create a new element in memory to store message
    char *w = new char[m_msg.size()+1];
    for (int i = 0; i < m_msg.size(); i++) {
        w[i] = m_msg.at(i).toAscii();
    }
    w[m_msg.size()] = 0;
    return  w;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////// QDebugger ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/**
  @brief    global function initialize/activate debugger
  @param    fileName    name of debug file to use
  **/
void DEBUG_INIT(QString fileName /* = DEBUGGER_STD_FILENAME */) {
    if (gDebug) {
        delete gDebug;
    }
    gDebug = new QDebugger(fileName);
}

/**
  @brief    global function to cleanly shut down debugging
  **/
void DEBUG_CLOSE() {
    if (gDebug) {
        gDebug->closeFile();
        delete gDebug;
        gDebug = NULL;
    }
}

/**
  @brief    initialize debugger object
  @param    fileName    name of a file to write debug messages to
  **/
QDebugger::QDebugger(const QString &fileName /*= QString()*/) : QObject(0)
,m_iLevelConsole(DEBUGGER_STD_LEVEL)
       ,m_iLevelGui(DEBUGGER_GUI_LEVEL)
       ,m_iLevelFile(DEBUGGER_STD_LEVEL)
       ,m_iCounterFile(0)
       ,m_iCounterConsole(0)
{
    setFileName(fileName);
}

/**
  @brief    destructor

  Ensures proper close-down of debugger and flushes file
  **/
QDebugger::~QDebugger()
{
    closeFile();
}

/**
  @brief    open file for debugging
  @param    fileName        absolute or relative file name for logging debug messages

  If fileName.isEmpty() then the file debugging feature is disabled
  **/
void QDebugger::setFileName(const QString &fileName)
{
    if (fileName.isEmpty())   {
        closeFile();
    } else {
        m_file.setFileName(fileName);
        if(!m_file.open(QIODevice::ReadWrite | QIODevice::Append)) {
            debug(QString("Error opening debug file \"%1\"!").arg(fileName),-1);
        }
        outFile("---- Application started! ----");
    }
}


/**
  @brief    shut down debugging to file
  **/
void QDebugger::closeFile()
{
    if (m_file.isOpen()) {
        outFile("---- Debugger clean shutdown. Closing log file! ----\n\n");
        m_file.flush();
        m_file.close();
    }
}

/**
  @brief    write some string to file
  @param    str debug message
  **/
void QDebugger::outFile(const QString &str)
{
#ifdef DEBUGGER_MULTITHREADING
    QMutexLocker locker(&m_mutex);
    Q_UNUSED(locker);
#endif
    if (m_file.isOpen()) {
#if DEBUGGER_TIMESTAMP
        QString line = QString("%1 %2: %3\n").arg(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")).arg(m_iCounterFile++).arg(str);
#else
        QString line = QString("%1: %2\n").arg(m_iCounterFile++).arg(str);
#endif
        m_file.write( QByteArray(line.toAscii()) );
#if DEBUGGER_PARANOID_FLUSH
        m_file.flush();
#endif
    } else {
		outConsole("Error: debug file not open/writeable");
		outConsole(str);
	}
}

/**
  @brief    display a debugging message box
  @param    str debug message
  **/
void QDebugger::outGui(const QString &str)
{
#if EXCEPTION_USE_GUI
    QMessageBox::warning(qApp->activeWindow(), "Debug Message", str);
#endif
}

/**
  @brief    message to console
  @param    str debug message
  **/
void QDebugger::outConsole(const QString &str)
{
#if DEBUGGER_TIMESTAMP
        QString line = QString("%1 %2: %3\n").arg(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")).arg(m_iCounterFile++).arg(str);
#else
        QString line = QString("%1: %2\n").arg(m_iCounterFile++).arg(str);
#endif
    printf( line.toAscii().data() );
    fflush(stdout);
    emit console(line);
}

/**
  @brief    the SHOULD USE function for debugging
  @param    msg debug message
  @param    level   message level

  According to the set levels, all debug channels are handled
  **/
void QDebugger::debug(const QString &msg, int level)
{
    if (NULL == this) {
        qDebug() << "Error: Debugger destructed before issuing message: " << msg;
        return;
    }
    if (level <=  m_iLevelFile) {
        outFile(msg);
    }
    if (level <= m_iLevelConsole) {
        outConsole(msg);
    }
#if EXCEPTION_USE_GUI
    if (level <= m_iLevelGui) {
        outGui(msg);
    }
#endif
}
