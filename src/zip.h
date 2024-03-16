#include <QString>

namespace CubesZip
{
    enum class ZipMethod
    {
        Create,
        Append
    };

    bool ZipFile(const QByteArray& byteArray, const QString& srcFileName, const QString& dstFilePath, ZipMethod method);

    bool ZipFile(const QString& srcFilePath, const QString& dstFilePath, ZipMethod method);

    bool GetZippedFileNames(const QString& zipFilePath, QList<QString>& fileNames);

    bool UnZipFile(const QString& zipFilePath, const QString& srcFileName, QByteArray& byteArray);
}
