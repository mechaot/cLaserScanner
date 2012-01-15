/**
    @file		matrix_qtascii.hpp
    @author	Andreas Poesch (itdevel@mechaos.de)
    @since      2010/02/22
    @warning 	ALPHA VERSION, not all functions may be numerically stable or working properly

    Simple Math Library (SiMaLi)

    Routines that are associated with human readable text, as ascii load and save functions or formatted output.
    These functions are programmed by help of QT libs.

    rows*cols may not exceed 2^31-1

    $Id: matrix_qtascii.hpp 73 2011-05-31 13:14:31Z mechaot $
**/



#include <QString>
#include <QChar>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QByteArray>

#include <opencv.hpp>

#include "QtException.h"

/**
  @brief    construct matrix from QString containing matrix ascii-data
  @param    str string with data


  **/
template<typename type> Matrix<type>::Matrix(const QString& str)		 // Constructor by ascii string
{
    LoadFromString(str);
}

/**
  @brief    associate Matrix with a name
  @param    name        name to use
**/
template<typename type> void Matrix<type>::setName(const QString& name)
{
    strName = name;
}

/**
  @brief    get Matrix name
  @return   string containing the matrix' name
**/
template<typename type> QString Matrix<type>::name()
{
    return strName;
}


/**
  @brief    conveniently check matrix name
  @param    name    string to check name against
  @return   returns true if name matches the matrix' name

  Comparison is case sensitive
**/
template<typename type> bool Matrix<type>::hasName(const QString& name)
{
    return (strName.compare(name, Qt::CaseSensitive) == 0);
}

/**
  @brief    load matrix either from ascii-string or file
  @param    datasource  string containing data for file reference
  @return
  **/
template<typename type> Matrix<type>& Matrix<type>::LoadMultisource(const QString& dataorsource)
{
    QString s(dataorsource.trimmed());
    if (s.startsWith("file:")) {
        QString fileName = s.remove("file:");
        LoadFromAsciiFile(fileName);
        setName(fileName);
    } else {
        LoadFromString(dataorsource);
    }
    return *this;
}

/**
  @brief    load from GenericMatrixData-File
  **/
template<typename type> Matrix<type>& Matrix<type>::LoadFromGMDFile(const QString &fileName)
{
    EX_THROW("Not ready");
    Clear();
    if (!QFile::exists(fileName)) {
        DEBUG(1,QString("File %1 not found!").arg(fileName));
        return *this;
    }
    int width = 0;
    int height = 0;
    int channels = 0;

    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::ReadOnly )) {
        DEBUG(1,QString("Error opening file %1 for reading!").arg(fileName));
        Clear();
        return *this;
    }
    //qDebug() << "File size is: " << file.size() << ". File status: opened.";
    QTextStream in(&file);
    QStringList list;

    QString line; // = in.readLine();

    //while (!line.isNull()) {
    while( !(line = in.readLine()).isNull() ) {
        line = line.trimmed();
        if (line.startsWith("#") || line.isEmpty()) { //comment line or empty line
            //line = in.readLine();
            continue;
        }
        //qDebug() << "Trimmed line: " << line;
        list.append(line);
        //line = in.readLine();
    }

}

/**
    @brief	load RECTANGULAR matrix from file
    @param	fileName    path and name of file
    @return	reference to self

    The matrix is resized to fit the dimensions in the string, so size needs not to be known
**/
template<typename type> Matrix<type>& Matrix<type>::LoadFromAsciiFile(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        qDebug() << "File \"" << fileName << "\" does not exist. Aborting...";
        Clear();
        return *this;
    }
    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::ReadOnly )) {
        qDebug() << "Error opening file \"" << fileName << "\".";
        Clear();
        return *this;
    }
    //qDebug() << "File size is: " << file.size() << ". File status: opened.";
    QTextStream in(&file);
    QStringList list;

    QString line; // = in.readLine();

    //while (!line.isNull()) {
    while( !(line = in.readLine()).isNull() ) {
        line = line.trimmed();
        if (line.startsWith("#") || line.isEmpty()) { //comment line or empty line
            //line = in.readLine();
            continue;
        }
        //qDebug() << "Trimmed line: " << line;
        list.append(line);
        //line = in.readLine();
    }
    return LoadFromStringList(list);

    QFileInfo info(file);
    strName = info.fileName();
    return *this;
}


/**
    @brief	load RECTANGULAR matrix from file
    @param	fileName    path and name of file
    @return	reference to self

    The matrix is resized to fit the dimensions in the string, so size needs not to be known

    semicolon as well as newline characters are treated as line separators
**/
template<typename type> Matrix<type>& Matrix<type>::LoadFromString(const QString &data)
{
    //qDebug() << "File size is: " << file.size() << ". File status: opened.";
    QString str = QString(data).replace(";","\n");        //allow ";" as line separator, too

    QTextStream in(&str);
    QStringList list;

    QString line; // = in.readLine();

    //while (!line.isNull()) {
    while( !(line = in.readLine()).isNull() ) {
        line = line.trimmed();
        if (line.startsWith("#") || line.isEmpty()) { //comment line or empty line
            //line = in.readLine();
            continue;
        }
        //qDebug() << "Trimmed line: " << line;
        list.append(line);
        //line = in.readLine();
    }
    return LoadFromStringList(list);
}

/**
  @brief    load from stringlist each containing one data row
  @param    in      list of strings
  @return   pointer to changed this
  **/
template<typename type> Matrix<type>& Matrix<type>::LoadFromStringList(const QStringList &list)
{
    int iCols = 0;
    //qDebug() << "Lines in file:" << lines_in_file << ". Data lines:" << list.size();

    int iRows = list.size();
    for(int i = 0; i < iRows; i++) {
        QStringList strCols = list.at(i).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (iCols) {    //not first line
            if (strCols.size() != iCols) {
                //qDebug() << "Error: Matrix not rectangular";
            }
        } else {    //first line, not yet determined number of elements
            iCols = strCols.size();
            Create(iCols, iRows);
            //qDebug() << "Creating matrix with " << iRows << " lines and " << iCols << "columns.";
        }
        for(int c = 0; c < strCols.size(); c++) {
            pData[i * iCols + c] = strCols.at(c).toDouble();
            //qDebug() << strCols.at(c) << "=" << pData[i * iCols + c];
        }
    }
    return *this;
}

/**
    @brief	load triangle vertices from blender .raw file export
    @param	fileName    path and name of file
    @return	reference to self

    File format:
    Each line contains 9 coords (3x x,y,z) for a triangular vertex or 12 coords for quadrangle (4x x,y,z), we convert these to 2 triangles using 1,2,3 and 2,3,4 as corners
**/
template<typename type> Matrix<type>& Matrix<type>::LoadFromRawTriangleVerticesFile(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        qDebug() << "File \"" << fileName << "\" does not exist. Aborting...";
        Clear();
        return *this;
    }
    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::ReadOnly )) {
        qDebug() << "Error opening file \"" << fileName << "\".";
        Clear();
        return *this;
    }
    //qDebug() << "File size is: " << file.size() << ". File status: opened.";


    int iRows = 0;
    int iCols = 9;          //3d triangles
    int lines_in_file = 0;

    QTextStream in(&file);

   // QString line;


    QTime tic = QTime::currentTime();
    QByteArray content = file.readAll();
    //file.seek(0);
    DEBUG(1,QString("A: %1ms").arg(tic.msecsTo(QTime::currentTime())));
    QStringList lines = QString(content).split('\n',QString::SkipEmptyParts);
    DEBUG(1,QString("B: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    //count lines, bad performance code
    foreach(QString line, lines) {            //parse each line
        line = line.trimmed();
        ++lines_in_file;
        if (line.startsWith("#") || line.isEmpty()) { //comment line or empty line
            continue;
        }
        QStringList strCols = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (strCols.count() == 9) {  //we have a 9 coordinates data->triangle
            ++iRows;
        } else if (strCols.count() == 12) { //12 coordinate data -> quadrangle; make it 2 triangles
            iRows += 2;
        } else {
            qDebug() << QString("Error parsing line %1; it has %2 data fields; ignoring line").arg(lines_in_file).arg(strCols.count());
            continue;
        }
    }
    DEBUG(1,QString("C: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    Create(iCols, iRows);
    //file.seek(0);
    int curRow = 0;

    //parse all lines of file
    foreach (QString line,lines) {            //parse each line
        line = line.trimmed();
        ++lines_in_file;
        if (line.startsWith("#") || line.isEmpty()) { //comment line or empty line
            continue;
        }
       //QStringList strCols = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QStringList strCols = line.split(QRegExp(" "), QString::SkipEmptyParts);
        if (strCols.count() == 9) {  //we have a 9 coordinates data->triangle
            for(int c = 0; c < 9; c++) {    //parse all columns that are conveniently aligned
                pData[curRow * iCols + c] = strCols.at(c).toDouble();
            }
            ++curRow;
        } else if (strCols.count() == 12) { //12 coordinate data -> quadrangle; make it 2 triangles
            for(int c = 0; c < 9; c++) {    //parse all columns that are conveniently aligned
                pData[curRow * iCols + c] = strCols.at(c).toDouble(); //corners 0,1,2
            }
            ++curRow;
            for(int c = 0; c < 9; c++) {    //parse all columns that are conveniently aligned
                pData[curRow * iCols + c] = strCols.at(c+3).toDouble(); //corners 1,2,3
            }
            ++curRow;
        } else {
            qDebug() << QString("Error parsing line %1; it has %2 data fields; ignoring line").arg(lines_in_file).arg(strCols.count());
            continue;
        }
    }
    DEBUG(1,QString("D: %1ms").arg(tic.msecsTo(QTime::currentTime())));
    QFileInfo info(file);
    strName = info.fileName();  //name matrix after filename
    return *this;
}

/**
    @brief	load triangle vertices from blender .raw file export
    @param	fileName    path and name of file
    @return	reference to self

    File format:
    Each line contains 9 coords (3x x,y,z) for a triangular vertex or 12 coords for quadrangle (4x x,y,z), we convert these to 2 triangles using 1,2,3 and 2,3,4 as corners
**/
template<typename type> Matrix<type>& Matrix<type>::LoadRaw(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        DEBUG(1,QString("Error file does not exist: '%1'.").arg(fileName));
        Clear();
        return *this;
    }
    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::ReadOnly )) {
        DEBUG(1,QString("Error opening file for reading: '%1'.").arg(fileName));;
        Clear();
        return *this;
    }

    QTime tic = QTime::currentTime();

    //DEBUG(1,QString("A: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    int rowcount = 0;
    int colcount = 0;   //for all triangles
    int bytesPerElement = 0;

    bool headercomplete = false;
    while(!headercomplete) {
        QByteArray binaryline(file.readLine());
        QString line(binaryline);
        line = line.trimmed();
        if (line.startsWith("#")) { //ignore lines starting with #
            continue;
        }
        if (line.isEmpty()) { //an empty line signals the header is finished
            headercomplete = true;
            break;
        }
        if (line.startsWith("rows:")) {
            rowcount = line.remove("rows:").toInt();
        }
        if (line.startsWith("cols:")) {
            colcount = line.remove("cols:").toInt();
        }
        if (line.startsWith("type:")) {
            bytesPerElement = line.remove("type:").toInt();
            if (!(bytesPerElement == sizeof(type))) {
                DEBUG(1,"Error: invalid matrix data type, or file corrupted");
            }
        }
    }

    int binsize = colcount * rowcount * sizeof(type);
    //DEBUG(1,QString("ParsedHeader: %1ms (%2 triangles, %3 bytes)").arg(tic.msecsTo(QTime::currentTime())).arg(rowcount).arg(binsize));
    if (binsize == 0) {
        DEBUG(1,"Error: attempt to read emtpy matrix. Aborting.");
    }

    Create(colcount,rowcount);
    int read = file.read((char*)this->pData, binsize);
    if (read != binsize) {
        DEBUG(1,QString("Error: could only read %1 bytes from %2 bytes requested").arg(read).arg(binsize));
    } else {
        //DEBUG(1,QString("Successfully read %1 bytes").arg(read));
    }
    //DEBUG(1,QString("LoadedBinary: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    return *this;
}


/**
    @brief	load triangle vertices from blender .raw file export
    @param	fileName    path and name of file
    @return	reference to self

    File format:
    Each line contains 9 coords (3x x,y,z) for a triangular vertex or 12 coords for quadrangle (4x x,y,z), we convert these to 2 triangles using 1,2,3 and 2,3,4 as corners
**/
template<typename type> bool Matrix<type>::SaveRaw(const QString &fileName) const
{
    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate )) {
        DEBUG(1,QString("Error opening file for saving: '%1'.").arg(fileName));
        return false;
    }

    file.write( QString("# Matrix '%1'\n").arg(strName).toAscii() );
    file.write( QString("rows: %1\ncols:%2\n\n").arg(rows).arg(cols).toAscii());

    int binsize = rows * cols * sizeof(type);

    int written = file.write((const char*)this->pData, binsize);
    if (written != binsize) {
        DEBUG(1,QString("Error: could only write %1 bytes to file from %2 bytes requested").arg(written).arg(binsize));
        return true;
    } else {
        //DEBUG(1,QString("Successfully read %1 bytes").arg(read));
        return false;
    }
}

/**
    @brief	load triangle vertices from blender .raw file export
    @param	fileName    path and name of file
    @return	reference to self

    File format:
    Each line contains 9 coords (3x x,y,z) for a triangular vertex or 12 coords for quadrangle (4x x,y,z), we convert these to 2 triangles using 1,2,3 and 2,3,4 as corners
**/
template<typename type> Matrix<type>& Matrix<type>::LoadFromBinaryTriangles(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        qDebug() << "File \"" << fileName << "\" does not exist. Aborting...";
        Clear();
        return *this;
    }
    QFile file(fileName);
    //if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if (!file.open(QIODevice::ReadOnly )) {
        qDebug() << "Error opening file \"" << fileName << "\".";
        Clear();
        return *this;
    }

    QTime tic = QTime::currentTime();

    //DEBUG(1,QString("A: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    int rowcount = 0;
    int colcount = 9;   //for all triangles

    bool headercomplete = false;
    while(!headercomplete) {
        QByteArray binaryline(file.readLine());
        QString line(binaryline);
        line = line.trimmed();
        if (line.startsWith("#")) { //ignore lines starting with #
            continue;
        }
        if (line.isEmpty()) { //an empty line signals the header is finished
            headercomplete = true;
            break;
        }
        if (line.startsWith("triangles:")) {
            rowcount = line.remove("triangles:").toInt();
        }
    }

    int binsize = colcount * rowcount * sizeof(type);
    //DEBUG(1,QString("ParsedHeader: %1ms (%2 triangles, %3 bytes)").arg(tic.msecsTo(QTime::currentTime())).arg(rowcount).arg(binsize));

    Create(colcount,rowcount);
    int read = file.read((char*)this->pData, binsize);
    if (read != binsize) {
        DEBUG(1,QString("Error: could only read %1 bytes from %2 bytes requested").arg(read).arg(binsize));
    } else {
        //DEBUG(1,QString("Successfully read %1 bytes").arg(read));
    }
    //DEBUG(1,QString("LoadedBinary: %1ms").arg(tic.msecsTo(QTime::currentTime())));

    return *this;
}
/**
    @brief	load RECTANGULAR matrix from SDF file
    @param	fileName    path and name of file
    @return	reference to self

    @bug header decoding is highly experimental

    The matrix is resized to fit the dimensions in the string, so size needs not to be known
**/
template<typename type> Matrix<type>& Matrix<type>::LoadFromSdfFile(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        qDebug() << "File \"" << fileName << "\" does not exist. Aborting...";
        Clear();
        return *this;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly )) {
        qDebug() << "Error opening file \"" << fileName << "\".";
        Clear();
        return *this;
    }
    //qDebug() << "File size is: " << file.size() << ". File status: opened.";


    int iRows = 0;
    int iCols = 0;

    int dataformat = 0;

    /* read header into byte array*/
    QByteArray header = file.read( 81 );
    unsigned char *ptr = (unsigned char*) header.data();     //get pointer to char data

    ASSERT( header.size() == 81);

    /* read size info from pointer */
    int tmp;
    tmp = ptr[43];
    iCols = ptr[42] + (tmp << 8);
    tmp = ptr[45];
    iRows = ptr[44] + (tmp << 8);

    double xScale = (double) *((double*) (ptr+46));
    double yScale = (double) *((double*) (ptr+54));
    double zScale = (double) *((double*) (ptr+62));

    dataformat = ptr[79];

    Create(3,iCols * iRows);
    file.seek(81);

    if (dataformat == 0x03) { // 32-bit real = float
        QByteArray filedata = file.read( sizeof(float) * iRows * iCols );
        float *data_ptr = (float*) filedata.data();

        int i = 0;
        for (int x = 0; x < iCols; x++) {
            for (int y = 0; y < iRows; y++) {
                i = y * iCols + ((iCols - 1) - x);    //to fit into the vtk-coordinate system x needs to be mirrored

                pData[i * 3] = xScale * x;
                pData[i * 3 + 1] = yScale * y;
                pData[i * 3 + 2] = data_ptr[i] * zScale;
            }
        }
    }
    else if (dataformat == 0x05) { // 16-bit signed int
        QByteArray filedata = file.read( sizeof(unsigned int) * iRows * iCols );
        unsigned int *data_ptr = (unsigned int*) filedata.data();

        int i;
        for (int x = 0; x < iCols; x++) {
            for (int y = 0; y < iRows; y++) {
                i = y * iCols + ((iCols - 1) - x);    //to fit into the vtk-coordinate system x needs to be mirrored

                pData[i * 3] = xScale * x;
                pData[i * 3 + 1] = yScale * y;
                pData[i * 3 + 2] = data_ptr[i] * zScale;
            }
        }
    }
    else if (dataformat == 0x07) { // 64-bit real
        QByteArray filedata = file.read( sizeof(double) * iRows * iCols );
        double *data_ptr = (double*) filedata.data();

        int i;
        for (int x = 0; x < iCols; x++) {
            for (int y = 0; y < iRows; y++) {
                i = y * iCols + ((iCols - 1) - x);    //to fit into the vtk-coordinate system x needs to be mirrored

                pData[i * 3] = xScale * x;
                pData[i * 3 + 1] = yScale * y;
                pData[i * 3 + 2] = data_ptr[i] * zScale;
            }
        }
    } else {
        Clear();
        CONSOLE(1,"Error: sdf-data raw-format not supported/unknown.");
    }

    return *this;
}


/**
    @brief	make a string from matrix
    @param	lineSep    line seperator, defaults to NEWLINE
    @return	string representing matrix in human readable format

    The matrix is resized to fit the dimensions in the string, so size needs not to be known
**/
template<typename type> QString Matrix<type>::ToString(const QString lineSep /* = '\n' */) const
{
    QString str;

    if (!strName.isEmpty())
        str.append(strName + " = ");
    str.append("[");
    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            str.append( QString("%1 ").arg(pData[r*cols + c]));
        }
        if (r < (rows-1))
            str.append(lineSep);
        else
            str.append("]\n");
    }
    return str;
}
