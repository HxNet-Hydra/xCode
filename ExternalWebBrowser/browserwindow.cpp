#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QStackedLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QPaintDevice>

BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile )
    : m_browser(browser)
    , m_profile(profile)
    , m_tabWidget(new TabWidget(profile, this))
    , m_progressBar(new QProgressBar(this))
    , m_historyBackAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_favAction(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    QToolBar *toolbar = createToolBar();
    toolbar2 = createToolBar2();
    menuBar()->addMenu(createViewMenu(toolbar));
    m_progressBar->setMaximumHeight(1);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"));
    m_tabWidget->setStyleSheet("QTabBar::tab { height: 0px; width: max; }");
    toolbar2->setStyleSheet("QToolBar { background-color : transparent ; color:white; border-color: transparent;}  QToolButton{background-color : transparent;} QToolButton { padding-left: 20px; padding-right: 10px; height:33;}");
    toolbar->setStyleSheet("QToolBar { background-color : transparent ; color:white; border-color: transparent;}  QToolButton{background-color : transparent;} ");

    QWidget *centralWidget = new QWidget(this);

    //set up layout
    layout3->addWidget(m_progressBar,0,0, Qt::AlignTop|Qt::AlignLeft);
    layout3->addWidget(m_tabWidget,0,0, Qt::AlignTop|Qt::AlignLeft);
    layout3->addWidget(toolbar,0,0, Qt::AlignTop|Qt::AlignLeft);
    layout3->addWidget(toolbar2,0,0, Qt::AlignTop|Qt::AlignRight);

    centralWidget->setLayout(layout3);
    setCentralWidget(centralWidget);

    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
    connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
    connect(m_tabWidget, &TabWidget::favIconChanged, m_favAction, &QAction::setIcon);

    //connect(&timer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    //set up socket
    m_socket=new QTcpSocket();
    wv = new WebView(this);
    //dw = new DownloadManagerWidget(this);

    wv->setFixedSize(0,0);

    connect(m_socket,&QTcpSocket::readyRead,this,&BrowserWindow::sockt_recv_data);

    for(int i=0;i<20;i++){
        if(!m_socket->waitForConnected()){
            m_socket->connectToHost("127.0.0.1", 00000);
        }
    }

    closeTab = new QAction(this);
    handleWebViewTitleChanged(QString());
    toolbar2->hide();
    m_tabWidget->createTab();
    connect(wv,SIGNAL(tab_detected(QString)),this,SLOT(tabDetected(QString)));
    connect(m_tabWidget,SIGNAL(closeSignal(QString)),this,SLOT(tabDetected(QString)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerSlot2()));
    timer.start(2*1000);
    //connect(dw,SIGNAL(tab_detected(QString)),this,SLOT(tabDetected(QString)));
}

void BrowserWindow::sockt_recv_data()
{
    //data receive through socket
    QByteArray data_tmp;
    data_tmp = m_socket->readLine();
    qDebug() << data_tmp;

    QStringList eventList = QString(data_tmp).split("#");
    foreach(QString str, eventList)
    {
        if(str.contains("Default UI Plugin") && str.contains("swap_widget_position")){
            QStringList elements  = str.split(";");
            QStringList elements2 = elements[2].split("&");

            foreach(QString element ,elements2)
            {
                if(element.contains("siqExternalWidget"))
                {
                    QStringList elements3 = element.split("=");
                    QStringList elements4 = elements3[1].split(",");
                    QString x2      = elements4[0];
                    QString y2      = elements4[1];
                    QString width2  = elements4[2];
                    QString height2 = elements4[3];
                    double x = x2.toDouble() - 10;
                    double y = y2.toDouble() - 10;
                    double width = width2.toDouble();
                    double height = height2.toDouble();

                    this->setGeometry(x,y,width,height);
                    this->currentTab()->setFixedSize(width,height);

                    break;
                }
            }
        }
        else if (str.contains("Signage Player") && str.contains("maximise_signage"))
        {
            this->hide();
        }
        else if (str.contains("Signage Player") && str.contains("minimise_signage"))
        {
            this->show();
        }
    }
}

QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu();
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    return viewMenu;
}

QToolBar *BrowserWindow::createToolBar2()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);


    QAction *closeTabAction = new QAction(tr("&Close Tab"), this);
    closeTabAction->setShortcuts(QKeySequence::Close);
    closeTabAction->setIconVisibleInMenu(false);
    closeTabAction->setIcon(QIcon(QStringLiteral(":close_button.png")));
    connect(closeTabAction, &QAction::triggered, [this]() {
        m_tabWidget->closeTab(m_tabWidget->currentIndex());
    });

    navigationBar->addAction(closeTabAction);

    return navigationBar;
}

void BrowserWindow::tabDetected(QString event)
{
    qDebug() << event;
    if(event == "download"){
        connect(closeTab, &QAction::triggered, [this]() {
            m_tabWidget->closeTab(m_tabWidget->currentIndex());
        });
        closeTab->trigger();
    }else if(event == "close"){
        qDebug() << "close";
        toolbar2->hide();


    }else if(event == "open"){
        qDebug() << "open";
        toolbar2->show();
    }
}

QToolBar *BrowserWindow::createToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);
    m_historyBackAction = new QAction(this);
    QList<QKeySequence> backShortcuts = QKeySequence::keyBindings(QKeySequence::Back);
    for (auto it = backShortcuts.begin(); it != backShortcuts.end();) {
        // Chromium already handles navigate on backspace when appropriate.
        if ((*it)[0] == Qt::Key_Backspace)
            it = backShortcuts.erase(it);
        else
            ++it;
    }
    // For some reason Qt doesn't bind the dedicated Back key to Back.
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    m_historyBackAction->setShortcuts(backShortcuts);
    m_historyBackAction->setIconVisibleInMenu(false);
    m_historyBackAction->setIcon(QIcon(QStringLiteral(":go-previous.png")));
    m_historyBackAction->setToolTip(tr("Go back in history"));
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });
    navigationBar->addAction(m_historyBackAction);

    m_stopReloadAction = new QAction(this);
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    return navigationBar;
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        m_historyBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        m_reloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        m_stopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}

void BrowserWindow::handleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(),
                                           tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty())
        return;
    currentTab()->setUrl(url);
}

void BrowserWindow::timerSlot2()
{
    QSettings settings("",QSettings::NativeFormat);
    QString s = settings.value( "download", "0" ).toString();
    if(s.isEmpty()){
        return;
    }else if(s == "download"){
        connect(closeTab, &QAction::triggered, [this]() {
            m_tabWidget->closeTab(m_tabWidget->currentIndex());
        });
        closeTab->trigger();
    }
    settings.setValue("download","");
    timer.start(2*1000);
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    QString suffix = m_profile->isOffTheRecord()
            ? tr("Qt Simple Browser (Incognito)")
            : tr("Qt Simple Browser");

    if (title.isEmpty())
        setWindowTitle(suffix);
    else
        setWindowTitle(title + " - " + suffix);
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

TabWidget *BrowserWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":process-stop.png"));
    static QIcon reloadIcon(QStringLiteral(":view-refresh.png"));

    if (0 < progress && progress < 100) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_progressBar->setValue(progress);
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_progressBar->setValue(0);
    }
}

void BrowserWindow::handleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QVector<BrowserWindow*> windows = m_browser->windows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->currentTab()->setFocus();
    }
}
