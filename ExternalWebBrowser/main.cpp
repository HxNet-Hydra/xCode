#include "browser.h"
#include "browserwindow.h"
#include "webview.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QCoreApplication>
#include <QDomElement>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo>
#include <QPushButton>
#include <QTextCodec>
#include <QSettings>

QUrl getCommandLineUrlArgument()
{
    const QStringList args = QCoreApplication::arguments();
    if (args.count() > 1)
        return QUrl::fromUserInput(args.last());
    return QUrl();
}


int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(QStringLiteral(":AppLogoColor.png")));

    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    Browser browser;
    BrowserWindow *window = browser.createWindow();

    QDesktopWidget QDW;
    QString layout_file;
    if(QDW.width() > QDW.height()){
        layout_file = QCoreApplication::applicationDirPath() + "\\layout.xml";
        //button->setGeometry(1255,506,0,0);
    }
    else{
        layout_file = QCoreApplication::applicationDirPath() + "\\layout.xml";
        //button->setGeometry(1255,506,0,0);
    }

    QString filename;
    filename = layout_file;

    QFile fExist(filename);
    if(!fExist.exists())
    {
        filename = QCoreApplication::applicationDirPath() + "\\layout.xml";
    }

    QString url2;
    QString sale_x;
    QString sale_y;
    QString sale_w;
    QString sale_h;
    QFile file(filename);
    if(file.open(QFile::ReadOnly))
    {
        while(!file.atEnd()) {
            QByteArray ba = file.readAll();
            QString content = QTextCodec::codecForMib(106)->toUnicode(ba); // to read unicode text file correctly

            QDomDocument doc;
            doc.setContent(content);

            QDomElement dom = doc.firstChildElement("layout").firstChildElement();
            while(!dom.isNull()) {
                QString tagName = dom.tagName();

                if(tagName == "item"){
                    QDomElement domElem = dom.firstChildElement();
                    while(!domElem.isNull()){
                        QString childTag = domElem.tagName();
                        QString childText = domElem.text();
                        if(childTag == "plugin_name"){
                            if(childText == "Browser Plugin")
                            {
                                QDomElement domElem_xy = dom.firstChildElement();
                                while(!domElem_xy.isNull()){
                                    QString childTag_xy = domElem_xy.tagName();
                                    QString childText_xy = domElem_xy.text();
                                    if(childTag_xy == "url")
                                        url2 = childText_xy;
                                    if(childTag_xy == "x")
                                        sale_x = childText_xy;
                                    if(childTag_xy == "y")
                                        sale_y = childText_xy;
                                    if(childTag_xy == "width")
                                        sale_w = childText_xy;
                                    if(childTag_xy == "height")
                                        sale_h = childText_xy;
                                    domElem_xy = domElem_xy.nextSiblingElement();
                                }

                                int cal_sale_x = (sale_x.toDouble() / 100) * QDW.geometry().width() -10;
                                int cal_sale_y = (sale_y.toDouble() / 100) * QDW.geometry().height() -10;
                                int cal_sale_w = (sale_w.toDouble() / 100) * QDW.geometry().width() ;
                                int cal_sale_h = (sale_h.toDouble() / 100) * QDW.geometry().height() ;

                                window->setGeometry(cal_sale_x,cal_sale_y,cal_sale_w,cal_sale_h);
                                window->currentTab()->setFixedSize(cal_sale_w,cal_sale_h);

                                QSettings settings("",QSettings::NativeFormat);
                                QString s;

                                if(settings.value( "machineUrl").toString().isEmpty()){
                                    s = "";
                                }else {
                                    s = settings.value( "machineUrl", "0" ).toString();
                                }
                                QUrl url = getCommandLineUrlArgument();
                                if (!url.isValid())
                                    url = url2.toLatin1();

                                qDebug() << s;
                                if(s == ""){
                                    window->currentTab()->setUrl(url);
                                }else{
                                    window->currentTab()->setUrl(s);
                                }
                                window->setAttribute( Qt::WA_TransparentForMouseEvents);
                            }
                        }
                        domElem = domElem.nextSiblingElement();
                    }
                }
                dom = dom.nextSiblingElement();
            }
        }
        // line space
        file.close();
    }
    return app.exec();
}




