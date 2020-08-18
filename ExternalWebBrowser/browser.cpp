#include "browser.h"
#include "browserwindow.h"
#include <QWebEngineProfile>
#include <QFile>
#include <QMenu>


Browser::Browser()
{
    // Quit application if the download manager window is the only remaining window
    m_downloadManagerWidget.setAttribute(Qt::WA_QuitOnClose, false);

    QObject::connect(
        QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
        &m_downloadManagerWidget, &DownloadManagerWidget::downloadRequested);
    QObject::connect(
        &m_otrProfile, &QWebEngineProfile::downloadRequested,
        &m_downloadManagerWidget, &DownloadManagerWidget::downloadRequested);
}

BrowserWindow *Browser::createWindow(bool offTheRecord)
{
    //create browser window on the mainwindow
    auto profile = offTheRecord ? &m_otrProfile : QWebEngineProfile::defaultProfile();
    auto mainWindow = new BrowserWindow(this, profile);
    m_windows.append(mainWindow);
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        m_windows.removeOne(mainWindow);
    });

    mainWindow->setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground);
    mainWindow->show();
    return mainWindow;
}
