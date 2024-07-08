/*
 * 文件管理类
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "filemgr.h"

FileMgr::FileMgr()
{

}

// 文件是否存在
bool FileMgr::isFileExist(const QString &sFilePath)
{
    // 是否传入了空的路径
    if (sFilePath.isEmpty())
        return false;

    return QFile().exists(sFilePath);
}

bool FileMgr::mkpath(const QString &path)
{
    QDir dir;
    return dir.mkpath(path);
}

bool FileMgr::rmpath(const QString &path)
{
    QDir dir;
    return dir.rmpath(path);
}

// 创建文件夹
bool FileMgr::createDir(QString sDirPath)
{
    QStringList dirNameArray = sDirPath.split('/');
    int nameSize = dirNameArray.size();
    for(int i=1; i<nameSize+1; i++)
    {
        QString iBefAllDirStr = "";
        for(int j=0; j<i; j++)
        {
            iBefAllDirStr += QString(dirNameArray.at(j) + '/');
        }

        QDir diri(iBefAllDirStr);
        if(diri.exists() == false) {
            diri.mkdir(iBefAllDirStr);
        }
    }

    return true;
}

// 复制文件
bool FileMgr::copyFile(const QString &sSrcFile, const QString &sDestFile, const bool &bCover)
{
    // 是否传入了空的路径||源文件是否存在
    if (sSrcFile.isEmpty() || sDestFile.isEmpty() || !QFile().exists(sSrcFile))
        return false;
    // 源文件路径与目标路径相同
    if(sSrcFile == sDestFile)
        return true;

    // 判断目标文件的目录存不存在，不存在则创建
    QFileInfo fileInfo(sDestFile);
    QString sDirPath = fileInfo.absolutePath(); // 取目标文件所在的绝对目录路径
    if(!QDir().exists(sDirPath))
        createDir(sDirPath);

    // 如果文件存在，允许覆盖，则删除目标路径文件
    QFile sTempFile(sDestFile);
    if(sTempFile.exists()) {
        if(bCover)
            sTempFile.remove();
    }

    // 复制文件
    QFile file(sSrcFile);
    return file.copy(sDestFile);
}

// 移动文件
bool FileMgr::moveFile(const QString &sSrcFile, const QString &sDestFile, const bool &bCover)
{
    // 是否传入了空的路径||源文件是否存在
    if (sSrcFile.isEmpty() || sDestFile.isEmpty() || !QFile().exists(sSrcFile))
        return false;
    // 源文件路径与目标路径相同
    if(sSrcFile == sDestFile)
        return true;

    // 允许覆盖，如果文件存在，则删除目标路径文件
    if(bCover) {
        QFile file(sDestFile);
        if(file.exists()) {
            if(!file.remove()) // 删除失败则返回false
                return  false;
        }
    }

    // 判断目标文件的目录存不存在，不存在则创建
    QFileInfo fileInfo(sDestFile);
    QString sDirPath = fileInfo.absolutePath(); // 取目标文件所在的绝对目录路径
    if(!QDir().exists(sDirPath))
        createDir(sDirPath);

    // 如果文件存在，允许覆盖，则删除目标路径文件
    QFile sTempFile(sDestFile);
    if(sTempFile.exists()) {
        if(bCover)
            sTempFile.remove();
    }

    // 移动文件
    QFile file(sSrcFile);
    return file.rename(sDestFile);
}

// 复制文件夹
bool FileMgr::copyDir(const QString &sSrcDir, const QString &sDestDir, const bool &bCover)
{
    // 是否传入了空的路径||源文件夹是否存在
    if (sSrcDir.isEmpty() || sDestDir.isEmpty() || !QDir().exists(sSrcDir))
        return false;

    QDir sourceDir(sSrcDir);
    QDir destDir(sDestDir);

    // 如果目标目录不存在，则进行创建
    if(!destDir.exists()) {
        if(!(createDir(destDir.absolutePath())))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        // 当为目录时，递归的进行copy
        if(fileInfo.isDir()) {
            if(!copyDir(fileInfo.filePath(),
                        destDir.filePath(fileInfo.fileName()),
                        bCover))
                return false;
        }
        else { // 当允许覆盖操作时，将旧文件进行删除操作
            if(bCover && destDir.exists(fileInfo.fileName())){
                destDir.remove(fileInfo.fileName());
            }

            // 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                            destDir.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }

    return true;
}

// 删除文件或文件夹
bool FileMgr::delFileOrDir(const QString &sPath) {
    //是否传入了空的路径||路径是否存在
    if (sPath.isEmpty() || !QDir().exists(sPath))
        return false;

    QFileInfo FileInfo(sPath);
    if (FileInfo.isFile()) // 如果是文件
        return QFile::remove(sPath);
    else if (FileInfo.isDir()) // 如果是文件夹
    {
        QDir qDir(sPath);
        return qDir.removeRecursively();
    }

    return true;
}

// 计算获取文件的MD5值
QByteArray FileMgr::getFileMd5(const QString &sFilePath)
{
    // 是否传入了空的路径||文件是否存在
    if (sFilePath.isEmpty() || !QFile().exists(sFilePath))
        return "";

    // 获取文件MD5值
    QFile md5File(sFilePath);
    md5File.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(md5File.readAll(), QCryptographicHash::Md5);
    md5File.close();
    return ba.toHex();
}
