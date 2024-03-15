#include <QString>

namespace CubesZip
{
    enum class ZipMethod
    {
        Create,
        Append
    };

    bool ZipFile(QString srcFilePath, QString dstFilePath, ZipMethod method);
}
