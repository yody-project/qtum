#include <qt/qtumversionchecker.h>
#include <clientversion.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

#define paternVersion "qtum-([0-9]+\\.)?([0-9]+\\.)?([0-9]+)-"

VuiCashVersionChecker::VuiCashVersionChecker(QObject *parent) : QObject(parent)
{
    currentVersion = Version(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION);
}

VuiCashVersionChecker::~VuiCashVersionChecker()
{

}

bool VuiCashVersionChecker::newVersionAvailable()
{
    Version maxReleaseVersion = getMaxReleaseVersion();
    return maxReleaseVersion > currentVersion;
}

QList<Version> VuiCashVersionChecker::getVersions()
{
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(QTUM_RELEASES)));
    QEventLoop event;
    connect(response, &QNetworkReply::finished, &event, &QEventLoop::quit);
    event.exec();
    QString html = response->readAll();

    QRegularExpression regEx(paternVersion);
    QRegularExpressionMatchIterator regExIt = regEx.globalMatch(html);

    QList<Version> versions;

    while (regExIt.hasNext()) {
        QRegularExpressionMatch match = regExIt.next();
        QString versionString = match.captured().mid(5, match.captured().length() - 6); // get version string in format XX.XX.XX
        Version version(versionString);
        if(!versions.contains(version))
        {
            versions.append(version);
        }
    }
    return versions;
}

Version VuiCashVersionChecker::getMaxReleaseVersion()
{
    QList<Version> versions = getVersions();
    Version maxVersion;

    if(!versions.isEmpty())
    {
        maxVersion = *std::max_element(versions.begin(), versions.end());
    }
    return maxVersion;
}
