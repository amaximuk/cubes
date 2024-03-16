#include <QFile>
#include <QFileInfo>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include "zip.h"

using namespace CubesZip;

bool CubesZip::ZipFile(const QByteArray& byteArray, const QString& srcFileName, const QString& dstFilePath, ZipMethod method)
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

bool CubesZip::ZipFile(const QString& srcFilePath, const QString& dstFilePath, ZipMethod method)
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

bool CubesZip::GetZippedFileNames(const QString& zipFilePath, QList<QString>& fileNames)
{
    zipFile zip = unzOpen(zipFilePath.toStdString().c_str());
    if (zip)
    {
        if (unzGoToFirstFile(zip) == UNZ_OK)
        {
            do
            {
                if (unzOpenCurrentFile(zip) == UNZ_OK)
                {
                    unz_file_info fileInfo{};

                    if (unzGetCurrentFileInfo(zip, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK)
                    {
                        std::vector<char> fileNameBuffer(fileInfo.size_filename);
                        unzGetCurrentFileInfo(zip, &fileInfo, fileNameBuffer.data(), fileNameBuffer.size(), NULL, 0, NULL, 0);

                        std::string fileName(fileNameBuffer.data(), fileNameBuffer.size());
                        fileNames.push_back(QString::fromStdString(fileName));
                    }

                    unzCloseCurrentFile(zip);
                }
            } while (unzGoToNextFile(zip) == UNZ_OK);
        }
        unzClose(zip);
    }

    return true;
}

bool CubesZip::UnZipFile(const QString& zipFilePath, const QString& srcFileName, QByteArray& byteArray)
{
    zipFile zip = unzOpen(zipFilePath.toStdString().c_str());
    if (zip)
    {
        if (unzGoToFirstFile(zip) == UNZ_OK)
        {
            do
            {
                if (unzOpenCurrentFile(zip) == UNZ_OK)
                {
                    unz_file_info fileInfo;
                    memset(&fileInfo, 0, sizeof(unz_file_info));

                    if (unzGetCurrentFileInfo(zip, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK)
                    {
                        std::vector<char> fileNameBuffer(fileInfo.size_filename);
                        unzGetCurrentFileInfo(zip, &fileInfo, fileNameBuffer.data(), fileNameBuffer.size(), NULL, 0, NULL, 0);

                        std::string fileName(fileNameBuffer.data(), fileNameBuffer.size());

                        if (QString::fromStdString(fileName) == srcFileName)
                        {
                            byteArray.resize(fileInfo.uncompressed_size);
                            int readBytes = unzReadCurrentFile(zip, byteArray.data(), byteArray.size());
                            if (readBytes != fileInfo.uncompressed_size)
                                return false;
                            else
                                return true;
                        }
                    }

                    unzCloseCurrentFile(zip);
                }
            } while (unzGoToNextFile(zip) == UNZ_OK);
        }
        unzClose(zip);
    }

    return false;


    //zipFile zip = unzOpen(zipFilePath.toStdString().c_str());
    //if (zip) {
    //    if (unzGoToFirstFile(zip) == UNZ_OK) {
    //        do {
    //            if (unzOpenCurrentFile(zip) == UNZ_OK) {
    //                unz_file_info fileInfo;
    //                memset(&fileInfo, 0, sizeof(unz_file_info));

    //                if (unzGetCurrentFileInfo(zip, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
    //                    char* filename = (char*)malloc(fileInfo.size_filename + 1);
    //                    unzGetCurrentFileInfo(zip, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
    //                    filename[fileInfo.size_filename] = '\0';

    //                    // At this point filename contains the full path of the file.
    //                    // If you only want files from a particular folder then you should compare
    //                    // against this filename and discards the files you don't want. 
    //                    //if (matchFolder(filename)) {
    //                    //    unsigned char buffer[4096];
    //                    //    int readBytes = unzReadCurrentFile(zip, buffer, 4096);
    //                    //    // Do the rest of your file reading and saving here.
    //                    //}

    //                    free(filename);
    //                }

    //                unzCloseCurrentFile(zip);
    //            }
    //        } while (unzGoToNextFile(zip) == UNZ_OK);
    //    }
    //    unzClose(zip);
    //}

    //return true;
}
