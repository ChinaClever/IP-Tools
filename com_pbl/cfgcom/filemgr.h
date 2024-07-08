#ifndef FILEMGR_H
#define FILEMGR_H
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>

class FileMgr
{
public:
    static FileMgr& build() {
        static FileMgr m_fileMgr;
        return m_fileMgr;
    }

    bool rmpath(const QString &path);
    bool mkpath(const QString &path);
    bool isFileExist(const QString &sFilePath); // 文件是否存在
    bool createDir(QString sDirPath); // 创建文件夹
    bool copyFile(const QString &sSrcFile, const QString &sDestFile, const bool &bCover); // 复制文件
    bool moveFile(const QString &sSrcFile, const QString &sDestFile, const bool &bCover); // 移动文件
    bool copyDir(const QString &sSrcDir, const QString &sDestDir, const bool &bCover); // 复制文件夹
    bool delFileOrDir(const QString &sPath); // 删除文件或文件夹
    QByteArray getFileMd5(const QString &sFilePath); // 计算获取文件的MD5值

private:
    FileMgr();
};

#endif // FILEMGR_H
