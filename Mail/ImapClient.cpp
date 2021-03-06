/**
 * @author  Mohammad S. Babaei <info@babaei.net>
 * @author  Mohamad mehdi Kharatizadeh <m_kharatizadeh@yahoo.com>
 */


#include <cassert>
#include <QString>
#if !defined ( Q_OS_ANDROID )
#include <vmime/vmime.hpp>
#include "BlindCertificateVerifier.hpp"
#elif defined(Q_OS_ANDROID)
#include "Android.hpp"
#endif // !defined ( Q_OS_ANDROID )]
#include "make_unique.hpp"
#include "ImapClient.hpp"
#include "Log.hpp"
#include "Message.hpp"
#include "Mailbox.hpp"
#include "../Pool.hpp"
#include <boost/thread.hpp>
#include "Json.hpp"
#include <QDebug>

#define         UNKNOWN_ERROR               "  ** ImapClient ->  Unknown Error!"

using namespace Ertebat;
using namespace Ertebat::Mail;

struct ImapClient::Impl
{
    Mail::SecurityType SecurityType;
    QString Host;
    Mail::Port_t Port;
    QString Username;
    QString Password;

#if !defined ( Q_OS_ANDROID )
    vmime::shared_ptr<vmime::net::session> Session;
    vmime::shared_ptr<vmime::net::store> Store;
    vmime::shared_ptr<vmime::net::transport> Transport;
    vmime::shared_ptr<Ertebat::Mail::BlindCertificateVerifier> BlindCertificateVerifier;
#endif // !defined ( Q_OS_ANDROID )

    Impl();
};

void ImapClient::GetMessageCountAsync() {

#if !defined(Q_OS_ANDROID)
    vmime::shared_ptr<vmime::net::folder> f = m_pimpl->Store->getDefaultFolder();
    f->open(vmime::net::folder::MODE_READ_WRITE);
    emit signal_GetMessageCountCompleted((int)f->getMessageCount());
#elif defined(Q_OS_ANDROID)
    emit signal_GetMessageCountCompleted((int)Pool::Android()->MailProfile_getMessageCount());
#endif

}


void ImapClient::FetchAsync(std::size_t from, std::size_t count) {
    QVector<Message> ret;

    try {

#if !defined(Q_OS_ANDROID)
        vmime::shared_ptr<vmime::net::folder> f = m_pimpl->Store->getDefaultFolder();
        f->open(vmime::net::folder::MODE_READ_WRITE);

        int c = f->getMessageCount();
        int to = std::max(0, c - ((int)(from + count)));
        for(int i = c - ((int)from); i > to; --i) {
            vmime::shared_ptr<vmime::net::message> msg = f->getMessage(i);

            f->fetchMessage(msg, vmime::net::fetchAttributes::FULL_HEADER);
            Message out;
            ExtractMessage(out, msg);
            ret.push_back(out);
        }


        emit signal_FetchCompleted(ret);
#elif defined(Q_OS_ANDROID)
        ////return m_pimpl->profile.fetchMessage((int) from, (int) count);
        auto v = Json::DecodeMessage(Pool::Android()->MailProfile_fetchMessage(from, count));
        for(auto i = v.begin(), _i = v.end(); i != _i; ++i) {
            ret.push_back(*i);
        }
        emit signal_FetchCompleted(ret);
#endif
    }
#if !defined ( Q_OS_ANDROID )
    catch (vmime::exception &ex) {
        LOG_ERROR(ex.what());
    }
#endif // !defined ( Q_OS_ANDROID )
    catch (std::exception &ex) {
        LOG_ERROR(ex.what());
    } catch(...) {
        LOG_ERROR(UNKNOWN_ERROR);
    }

}

void ImapClient::FetchAsJsonAsync(std::size_t from, std::size_t count)
{
    std::vector<Message> ret;

    try {

#if !defined(Q_OS_ANDROID)
        vmime::shared_ptr<vmime::net::folder> f = m_pimpl->Store->getDefaultFolder();
        f->open(vmime::net::folder::MODE_READ_WRITE);

        int c = f->getMessageCount();
        int to = std::max(0, c - ((int)(from + count)));
        for(int i = c - ((int)from); i > to; --i) {
            vmime::shared_ptr<vmime::net::message> msg = f->getMessage(i);

            f->fetchMessage(msg, vmime::net::fetchAttributes::FULL_HEADER);
            Message out;
            ExtractMessage(out, msg);
            ret.push_back(out);
        }


        std::ofstream fout("c:\\users\\kharatizadeh\\desktop\\json.txt");
        fout << Json::EncodeMessage(ret).toStdString();
        fout.close();

        onFetchJsonMessage(Json::EncodeMessage(ret));
#elif defined(Q_OS_ANDROID)
        onFetchJsonMessage(Pool::Android()->MailProfile_fetchMessage(from, count));
#endif
    }
#if !defined ( Q_OS_ANDROID )
    catch (vmime::exception &ex) {
        LOG_ERROR(ex.what());
    }
#endif // !defined ( Q_OS_ANDROID )
    catch (std::exception &ex) {
        LOG_ERROR(ex.what());
    } catch(...) {
        LOG_ERROR(UNKNOWN_ERROR);
    }

}

void ImapClient::getMessageCount()
{
    GetMessageCount();
}

void ImapClient::fetchAsJson(int i, int count)
{
    FetchAsJson(i, count);
}

ImapClient::ImapClient() :
    m_pimpl(std::make_unique<ImapClient::Impl>())
{
#if defined(Q_OS_ANDROID)
    ////m_pimpl->profile.init();
#endif
}

ImapClient::~ImapClient()
{

}

const Ertebat::Mail::SecurityType &ImapClient::GetSecurityType() const
{
    return m_pimpl->SecurityType;
}

void ImapClient::SetSecurityType(const Ertebat::Mail::SecurityType &securityType)
{
    m_pimpl->SecurityType = securityType;
}

const QString &ImapClient::GetHost() const
{
    return m_pimpl->Host;
}

void ImapClient::SetHost(const QString &host)
{
    m_pimpl->Host = host;
}

const Mail::Port_t &ImapClient::GetPort() const
{
    return m_pimpl->Port;
}

void ImapClient::SetPort(const Mail::Port_t &port)
{
    m_pimpl->Port = port;
}

const QString &ImapClient::GetUsername() const
{
    return m_pimpl->Username;
}

void ImapClient::SetUsername(const QString &username)
{
    m_pimpl->Username = username;
}

const QString &ImapClient::GetPassword() const
{
    return m_pimpl->Password;
}

void ImapClient::SetPassword(const QString &password)
{
    m_pimpl->Password = password;
}

// QML Hack
void ImapClient::setSecurityType(const int securityType)
{
    SetSecurityType(static_cast<Mail::SecurityType>(securityType));
}

// QML Hack
void ImapClient::setPort(const int &port)
{
    SetPort(static_cast<Mail::Port_t>(port));
}

void ImapClient::ConnectAsync()
{
    try {
#if !defined ( Q_OS_ANDROID )
        std::string protocol;
        switch (m_pimpl->SecurityType) {
        case Mail::SecurityType::SSL_TLS:
            protocol = "imaps";
            break;
        case Mail::SecurityType::None:
            // fall through
        case Mail::SecurityType::STARTTLS:
            // fall through
        default:
            protocol = "imap";
            break;
        }

        vmime::utility::url url(protocol,
                                m_pimpl->Host.toStdString(),
                                static_cast<vmime::port_t>(m_pimpl->Port),
                                std::string(""),
                                std::string(""),
                                std::string("")
                                );
        m_pimpl->Session = vmime::make_shared<vmime::net::session>();
        m_pimpl->Store = m_pimpl->Session->getStore(url);
        m_pimpl->Transport = m_pimpl->Session->getTransport(url);
        m_pimpl->BlindCertificateVerifier = vmime::make_shared<Ertebat::Mail::BlindCertificateVerifier>();

        if (m_pimpl->SecurityType == Mail::SecurityType::STARTTLS) {
            m_pimpl->Store->setProperty("connection.tls", true);
        }

        if (m_pimpl->Password != "") {
            m_pimpl->Store->setProperty("auth.username", m_pimpl->Username.toStdString().c_str());
            m_pimpl->Store->setProperty("auth.password", m_pimpl->Password.toStdString().c_str());
            m_pimpl->Store->setProperty("options.need-authentication", true);
        }

        if (m_pimpl->SecurityType != Mail::SecurityType::None) {
             m_pimpl->Store->setCertificateVerifier(m_pimpl->BlindCertificateVerifier);
        }

        m_pimpl->Store->connect();

#elif defined(Q_OS_ANDROID)

        qDebug("00000000000000");
        Pool::Android()->MailProfile_init();
        qDebug("a0000000000000");
        Pool::Android()->MailProfile_init();
        qDebug("b00000000000000");
        Pool::Android()->MailProfile_setHost(m_pimpl->Host, "imap");
        qDebug("c00000000000000");
        Pool::Android()->MailProfile_setPort(m_pimpl->Port, "imap");
        qDebug("d00000000000000");
        Pool::Android()->MailProfile_setUsername(m_pimpl->Username, "imap");
        qDebug("e00000000000000");
        Pool::Android()->MailProfile_setPassword(m_pimpl->Password, "imap");
        qDebug("f0000000000000");
        Pool::Android()->MailProfile_setSecurity(m_pimpl->SecurityType, "imap");
        qDebug("g0000000000000");
        qDebug() << Pool::Android()->MailProfile_connect("imap");
        qDebug("1111111111");

        //////////
//        m_pimpl->profile.init();
//        m_pimpl->profile.setHost(m_pimpl->Host, "imap");
//        m_pimpl->profile.setPort(m_pimpl->Port, "imap");
//        m_pimpl->profile.setUsername(m_pimpl->Username, "imap");
//        m_pimpl->profile.setPassword(m_pimpl->Password, "imap");
//        m_pimpl->profile.setSecurity(m_pimpl->SecurityType, "imap");
//        m_pimpl->profile.connect("imap");

#endif // !defined ( Q_OS_ANDROID )

        emit signal_ConnectCompleted(true);
    }
#if !defined ( Q_OS_ANDROID )
    catch (vmime::exception &ex) {
        LOG_ERROR(ex.what());
    }
#endif // !defined ( Q_OS_ANDROID )
    catch (std::exception &ex) {
        LOG_ERROR(ex.what());
    } catch(...) {
        LOG_ERROR(UNKNOWN_ERROR);
    }

    ///return false;
}

void ImapClient::DisconnectAsync()
{
    try {
#if !defined ( Q_OS_ANDROID )
        if (m_pimpl->Store != nullptr) {
            m_pimpl->Store->disconnect();
            emit signal_DisconnectCompleted();
        }
#elif defined(Q_OS_ANDROID)
        Pool::Android()->MailProfile_disconnect("imap");
        emit signal_DisconnectCompleted();
        /////////////m_pimpl->profile.disconnect("imap");
#endif // !defined ( Q_OS_ANDROID )
    }
#if !defined ( Q_OS_ANDROID )
    catch (vmime::exception &ex) {
        LOG_ERROR(ex.what());
    }
#endif // !defined ( Q_OS_ANDROID )
    catch (std::exception &ex) {
        LOG_ERROR(ex.what());
    } catch(...) {
        LOG_ERROR(UNKNOWN_ERROR);
    }
}

void ImapClient::Connect() {
    boost::thread t(static_cast<void (ImapClient::*)()>(&ImapClient::ConnectAsync), this);
    t.detach();
}

void ImapClient::Disconnect() {
    boost::thread t(static_cast<void (ImapClient::*)()>(&ImapClient::DisconnectAsync), this);
    t.detach();
}

void ImapClient::GetMessageCount() {
    boost::thread t(static_cast<void (ImapClient::*)()>(&ImapClient::GetMessageCountAsync), this);
    t.detach();
}

void ImapClient::Fetch(std::size_t i, std::size_t count) {
    boost::thread t(static_cast<void (ImapClient::*)(std::size_t, std::size_t)>(&ImapClient::FetchAsync), this, i, count);
    t.detach();
}

void ImapClient::FetchAsJson(std::size_t i, std::size_t count) {
    boost::thread t(static_cast<void (ImapClient::*)(std::size_t, std::size_t)>(&ImapClient::FetchAsJsonAsync), this, i, count);
    t.detach();
}

ImapClient::Impl::Impl()
{
    SecurityType = Mail::SecurityType::None;
    Host = "localhost";
    Port = 143;
}

