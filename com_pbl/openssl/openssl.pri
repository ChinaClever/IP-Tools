# Qt 5.15 Windows 不会把 OpenSSL 链进程序，运行时按文件名加载：
#   libssl-1_1-x64.dll、libcrypto-1_1-x64.dll
# 这两份库来自 OpenSSL 1.1.1t（MSVC），还依赖 vcruntime140.dll。
# 编译完成后复制到 exe 同目录，打包时随程序带走，目标电脑不必再安装 OpenSSL。

win32 {
    CONFIG(debug, debug|release) {
        OPENSSL_OUT = $$OUT_PWD/debug
    } else {
        OPENSSL_OUT = $$OUT_PWD/release
    }
    OPENSSL_OUT_WIN = $$shell_path($$OPENSSL_OUT)
    OPENSSL_SRC_WIN = $$shell_path($$PWD)

    QMAKE_POST_LINK += $$quote(cmd /c copy /Y \"$$OPENSSL_SRC_WIN\\libssl-1_1-x64.dll\" \"$$OPENSSL_OUT_WIN\\libssl-1_1-x64.dll\")
    QMAKE_POST_LINK += $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$quote(cmd /c copy /Y \"$$OPENSSL_SRC_WIN\\libcrypto-1_1-x64.dll\" \"$$OPENSSL_OUT_WIN\\libcrypto-1_1-x64.dll\")
    QMAKE_POST_LINK += $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$quote(cmd /c copy /Y \"$$OPENSSL_SRC_WIN\\vcruntime140.dll\" \"$$OPENSSL_OUT_WIN\\vcruntime140.dll\")
}
