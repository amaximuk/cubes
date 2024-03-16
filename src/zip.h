#include <QString>

namespace CubesZip
{
    enum class ZipMethod
    {
        Create,
        Append
    };

    bool ZipFile(QByteArray byteArray, QString srcFileName, QString dstFilePath, ZipMethod method);

    bool ZipFile(QString srcFilePath, QString dstFilePath, ZipMethod method);
}
