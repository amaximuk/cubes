#include <QFile>
#include <QFileInfo>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include "zip.h"

using namespace CubesZip;

bool CubesZip::ZipFile(QByteArray byteArray, QString srcFileName, QString dstFilePath, ZipMethod method)
{
    int append_status = APPEND_STATUS_CREATE;
    if (method == ZipMethod::Append)
        append_status = APPEND_STATUS_ADDINZIP;

    zipFile zf = zipOpen(dstFilePath.toStdString().c_str(), append_status);
    if (zf == NULL)
        return false;

    zip_fileinfo zfi{};
    zfi.internal_fa = 0;
    zfi.external_fa = 32u;
    zfi.dosDate = 0;
    const auto t = std::time(nullptr);
    const auto ptm = std::localtime(&t);
    zfi.tmz_date.tm_year = ptm->tm_year;
    zfi.tmz_date.tm_mon = ptm->tm_mon;
    zfi.tmz_date.tm_mday = ptm->tm_mday;
    zfi.tmz_date.tm_hour = ptm->tm_hour;
    zfi.tmz_date.tm_min = ptm->tm_min;
    zfi.tmz_date.tm_sec = ptm->tm_sec;

    if (S_OK == zipOpenNewFileInZip(zf, srcFileName.toStdString().c_str(), &zfi,
        NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
    {
        if (zipWriteInFileInZip(zf, byteArray.data(), byteArray.size()))
            return false;

        if (zipCloseFileInZip(zf))
            return false;
    }

    if (zipClose(zf, NULL))
        return false;

    return true;
}

bool CubesZip::ZipFile(QString srcFilePath, QString dstFilePath, ZipMethod method)
{
    QFile xmlFile(srcFilePath);
    if (!xmlFile.open(QIODevice::ReadOnly))
        return false;

    QByteArray byteArray = xmlFile.readAll();
    xmlFile.close();

    QFileInfo fi(srcFilePath);
    QString fileName = fi.fileName();

    return ZipFile(byteArray, fileName, dstFilePath, method);
}
