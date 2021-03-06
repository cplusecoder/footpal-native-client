/**
 * @author  Mohammad S. Babaei <info@babaei.net>
 * @author  Mohamad mehdi Kharatizadeh <m_kharatizadeh@yahoo.com>
 */


#include <mutex>
#include <cassert>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTranslator>
#include "make_unique.hpp"
#include "Pool.hpp"
#if defined ( Q_OS_ANDROID )
#include "Android.hpp"
#endif // defined ( Q_OS_ANDROID )
#include "Crypto.hpp"
#include "Database.hpp"
#include "HttpFileTransfer.hpp"
#include "RestApi.hpp"
#include "Mail/Mailbox.hpp"

/*
 *  PLEASE NOTE: WINDOWS SUPPORTS DIRECTORY SEPERATOR '/'
 *  WE CAN USE THE SAME FOR ANY PLATFORM
 *
 *  MEHDI KHARATIZADEH
 *
 */

#if defined ( _WIN32 )
#define     DATABASE_RELATIVE_PATH          "resources\\db"
#else
#define     DATABASE_RELATIVE_PATH          "resources/db"
#endif // defined ( _WIN32 )
#define     DATABASE_FILE_NAME              "database.db"

using namespace std;
using namespace Ertebat;

struct Pool::Impl
{
    typedef std::unique_ptr<Pool::StorageStruct> Storage_t;
#if defined ( Q_OS_ANDROID )
    typedef std::unique_ptr<Ertebat::Android> Android_t;
#endif // defined ( Q_OS_ANDROID )
    typedef std::unique_ptr<Ertebat::Crypto> Crypto_t;
    typedef std::unique_ptr<Ertebat::Database> Database_t;
    typedef std::unique_ptr<Ertebat::HttpFileTransfer> HttpFileTransfer_t;
    typedef std::unique_ptr<Ertebat::RestApi> RestApi_t;
    typedef std::unique_ptr<QTranslator> Translator_t;
    typedef std::unique_ptr<Ertebat::Mail::SmtpClient> SmtpClient_t;
    typedef std::unique_ptr<Ertebat::Mail::Pop3Client> Pop3Client_t;
    typedef std::unique_ptr<Ertebat::Mail::ImapClient> ImapClient_t;

    static std::mutex StorageMutex;
    static Storage_t StorageInstance;

#if defined ( Q_OS_ANDROID )
    static std::mutex AndroidMutex;
    static Android_t AndroidInstance;
#endif // defined ( Q_OS_ANDROID )

    static std::mutex CryptoMutex;
    static Crypto_t CryptoInstance;

    static std::mutex DatabaseMutex;
    static Database_t DatabaseInstance;

    static std::mutex HttpFileTransferMutex;
    static HttpFileTransfer_t HttpFileTransferInstance;

    static std::mutex RestApiMutex;
    static RestApi_t RestApiInstance;

    static std::mutex TranslatorMutex;
    static Translator_t TranslatorInstance;

    static std::mutex SmtpClientMutex;
    static SmtpClient_t SmtpClientInstance;

    static std::mutex Pop3ClientMutex;
    static Pop3Client_t Pop3ClientInstance;

    static std::mutex ImapClientMutex;
    static ImapClient_t ImapClientInstance;
};

std::mutex Pool::Impl::StorageMutex;
Pool::Impl::Storage_t Pool::Impl::StorageInstance = nullptr;

#if defined ( Q_OS_ANDROID )
std::mutex Pool::Impl::AndroidMutex;
Pool::Impl::Android_t Pool::Impl::AndroidInstance = nullptr;
#endif // defined ( Q_OS_ANDROID )

std::mutex Pool::Impl::CryptoMutex;
Pool::Impl::Crypto_t Pool::Impl::CryptoInstance = nullptr;

std::mutex Pool::Impl::DatabaseMutex;
Pool::Impl::Database_t Pool::Impl::DatabaseInstance = nullptr;

std::mutex Pool::Impl::HttpFileTransferMutex;
Pool::Impl::HttpFileTransfer_t Pool::Impl::HttpFileTransferInstance = nullptr;

std::mutex Pool::Impl::RestApiMutex;
Pool::Impl::RestApi_t Pool::Impl::RestApiInstance = nullptr;

std::mutex Pool::Impl::TranslatorMutex;
Pool::Impl::Translator_t Pool::Impl::TranslatorInstance = nullptr;

std::mutex Pool::Impl::SmtpClientMutex;
Pool::Impl::SmtpClient_t Pool::Impl::SmtpClientInstance = nullptr;

std::mutex Pool::Impl::Pop3ClientMutex;
Pool::Impl::Pop3Client_t Pool::Impl::Pop3ClientInstance = nullptr;

std::mutex Pool::Impl::ImapClientMutex;
Pool::Impl::ImapClient_t Pool::Impl::ImapClientInstance = nullptr;

Pool::StorageStruct *Pool::Storage()
{
    lock_guard<mutex> lock(Impl::StorageMutex);
    (void)lock;

    if (Impl::StorageInstance == nullptr) {
        Impl::StorageInstance =
                std::make_unique<Pool::StorageStruct>();
    }

    return Impl::StorageInstance.get();
}

#if defined ( Q_OS_ANDROID )
Ertebat::Android *Pool::Android()
{
    lock_guard<mutex> lock(Impl::AndroidMutex);
    (void)lock;

    if (Impl::AndroidInstance == nullptr) {
        Impl::AndroidInstance =
                std::make_unique<Ertebat::Android>();
    }

    return Impl::AndroidInstance.get();
}
#endif

Ertebat::Crypto *Pool::Crypto()
{
    lock_guard<mutex> lock(Impl::CryptoMutex);
    (void)lock;

    if (Impl::CryptoInstance == nullptr) {
        // Use this nice HEX/ASCII converter and your editor's replace dialog,
        // to create your own Key and IV.
        // http://www.dolcevie.com/js/converter.html
        // or
        // http://www.asciitohex.com/
        // To generate random password: https://strongpasswordgenerator.com/

        // bQN>S;989684747z
        static constexpr Crypto::Byte_t KEY[] = {
            0x62, 0x51, 0x4e, 0x3e, 0x53, 0x3b, 0x39, 0x38, 0x39, 0x36, 0x38, 0x34, 0x37, 0x34, 0x37, 0x7a
        };

        // p=qHWjj#42h}68^x
        static constexpr Crypto::Byte_t IV[] = {
            0x70, 0x3d, 0x71, 0x48, 0x57, 0x6a, 0x6a, 0x23, 0x34, 0x32, 0x68, 0x7d, 0x36, 0x38, 0x5e, 0x78
        };

        Impl::CryptoInstance =
                std::make_unique<Ertebat::Crypto>(KEY, sizeof(KEY), IV, sizeof(IV));
    }

    return Impl::CryptoInstance.get();
}

Ertebat::Database *Pool::Database()
{
    lock_guard<mutex> lock(Impl::DatabaseMutex);
    (void)lock;

    if (Impl::DatabaseInstance == nullptr) {
        QString dbPath(QDir::currentPath());
        if (!dbPath.endsWith(QDir::separator()))
            dbPath += QDir::separator();
        dbPath += DATABASE_RELATIVE_PATH;
        if (!dbPath.endsWith(QDir::separator()))
            dbPath += QDir::separator();
        QString dbFile(dbPath + DATABASE_FILE_NAME);

        if (!QFileInfo(dbFile).exists()) {
            if (!QDir(dbPath).exists()) {
                assert(QDir().mkpath(dbPath));
            }
            Database::Vacuum(dbFile.toStdString());
            assert(QFile::setPermissions(dbFile, QFile::ReadOwner | QFile::WriteOwner));
        }

#if defined ( STATIC_LINK_DEPENDENCIES )
        if (!Database::IsSQLite3DriverLoaded())
            Database::LoadSQLite3Driver();
#endif  // defined ( STATIC_LINK_DEPENDENCIES )

        Impl::DatabaseInstance =
                std::make_unique<Ertebat::Database>(QDir::toNativeSeparators(dbFile).toStdString());
    }

    return Impl::DatabaseInstance.get();
}

Ertebat::HttpFileTransfer *Pool::HttpFileTransfer()
{
    lock_guard<mutex> lock(Impl::HttpFileTransferMutex);
    (void)lock;

    if (Impl::HttpFileTransferInstance == nullptr) {
        Impl::HttpFileTransferInstance =
                std::make_unique<Ertebat::HttpFileTransfer>();
    }

    return Impl::HttpFileTransferInstance.get();
}

Ertebat::RestApi *Pool::RestApi()
{
    lock_guard<mutex> lock(Impl::RestApiMutex);
    (void)lock;

    if (Impl::RestApiInstance == nullptr) {
        Impl::RestApiInstance =
                std::make_unique<Ertebat::RestApi>();
    }

    return Impl::RestApiInstance.get();
}

QTranslator *Pool::Translator()
{
    lock_guard<mutex> lock(Impl::TranslatorMutex);
    (void)lock;

    if (Impl::TranslatorInstance == nullptr) {
        Impl::TranslatorInstance =
                std::make_unique<QTranslator>();
    }

    return Impl::TranslatorInstance.get();
}

Ertebat::Mail::SmtpClient* Pool::SmtpClient() {
    lock_guard<mutex> lock(Impl::SmtpClientMutex);
    (void)lock;

    if(Impl::SmtpClientInstance == nullptr) {
        Impl::SmtpClientInstance =
                std::make_unique<Ertebat::Mail::SmtpClient>();
    }

    return Impl::SmtpClientInstance.get();
}

Ertebat::Mail::Pop3Client* Pool::Pop3Client() {
    lock_guard<mutex> lock(Impl::Pop3ClientMutex);
    (void)lock;

    if(Impl::Pop3ClientInstance == nullptr) {
        Impl::Pop3ClientInstance =
                std::make_unique<Ertebat::Mail::Pop3Client>();
    }

    return Impl::Pop3ClientInstance.get();
}

Ertebat::Mail::ImapClient* Pool::ImapClient() {
    lock_guard<mutex> lock(Impl::ImapClientMutex);
    (void)lock;

    if(Impl::ImapClientInstance == nullptr) {
        Impl::ImapClientInstance =
                std::make_unique<Ertebat::Mail::ImapClient>();
    }

    return Impl::ImapClientInstance.get();
}
