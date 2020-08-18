#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>

#include <QWebEnginePage>
#include <QDesktopWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QTcpServer>
#include <QTimer>
#include <QTcpSocket>
#include <QRegularExpression>
#include <downloadmanagerwidget.h>
#include <downloadwidget.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QProgressBar;
QT_END_NAMESPACE

class Browser;
class TabWidget;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(Browser *browser, QWebEngineProfile *profile);
    QSize sizeHint() const override;
    TabWidget *tabWidget() const;
    WebView *currentTab() const;
    Browser *browser() { return m_browser; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void handleShowWindowTriggered();
    void handleWebViewLoadProgress(int);
    void handleWebViewTitleChanged(const QString &title);
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void sockt_recv_data();
    void handleFileOpenTriggered();
    void timerSlot2();

private:
    QMenu *createViewMenu(QToolBar *toolBar);
    QToolBar *createToolBar();
    QToolBar *createToolBar2();
    QVector<BrowserWindow*> m_windows;

private:
    Browser *m_browser;
    QWebEngineProfile *m_profile;
    TabWidget *m_tabWidget;
    QProgressBar *m_progressBar;
    QAction *m_historyBackAction;
    QAction *m_stopAction;
    QAction *m_reloadAction;
    QAction *m_stopReloadAction;
    QLineEdit *m_urlLineEdit;
    QAction *m_favAction;
    QString m_lastSearch;
    QDesktopWidget QDW;
    QGridLayout *layout3 = new QGridLayout();
    QTcpSocket *m_socket;
    QPushButton *image2 = new QPushButton(this);
    QTimer timer;
    WebView *wv;
    QAction *closeTab;
    QToolBar *toolbar2;

    //DownloadManagerWidget *dw;

private slots:
    void tabDetected(QString event);
};

#endif // BROWSERWINDOW_H
