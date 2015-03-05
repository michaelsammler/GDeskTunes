#include <QDebug>
#include <QSettings>
#include <QFileDialog>

#include "gdeskstyler.h"
#include "ui_gdeskstyler.h"

#include "QtColorPropertyManager"
#include "QtStringPropertyManager"

#include "QtLineEditFactory"
#include "QtColorEditorFactory"
#include "QtSpinBoxFactory"

GDeskStyler::GDeskStyler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GDeskStyler),
    style(0),
    filename(QString::null)
{
    ui->setupUi(this);

    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    groupManager = new QtGroupPropertyManager(this);

    QObject::connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(valueChanged(QtProperty *,QString)));
    QObject::connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(valueChanged(QtProperty *,QColor)));
    QObject::connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(valueChanged(QtProperty *,int)));

    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);

    ui->property_browser->setFactoryForManager(stringManager, lineEditFactory);
    ui->property_browser->setFactoryForManager(colorManager, colorEditorFactory);
    ui->property_browser->setFactoryForManager(intManager, spinBoxFactory);

    style = new GStyle();
    populate(style);
}

GDeskStyler::~GDeskStyler()
{
    delete ui;
}

void GDeskStyler::populate(QObject *object, QtProperty *parent)
{
    const QMetaObject* meta = object->metaObject();

    for(int i=1; i<meta->propertyCount(); ++i)
    {
        QtProperty *item = 0;
        QMetaProperty property = meta->property(i);
        QVariant variant = property.read(object);
        switch(variant.type())
        {
        case QVariant::String:
            item = stringManager->addProperty(property.name());
            break;
        case QVariant::Color:
            item = colorManager->addProperty(property.name());
            break;
        case QVariant::Int:
            item = intManager->addProperty(property.name());
            break;
         default:
            qDebug() << "Unsupported property" << property.name() << property.type();
            item = groupManager->addProperty(property.name());
            populate(variant.value<QObject*>(),item);
        }
        if (item)
        {
            if (parent)
                parent->addSubProperty(item);
            else
            {
                QtBrowserItem *it = ui->property_browser->addProperty(item);
                ui->property_browser->setExpanded(it, false);
            }
            holders[item] = object;
        }
    }

}

template <typename T>
void value_changed(QObject *holder, QtProperty *property, T& value)
{
    if (holder == 0)
    {
        qDebug() << "No target object";
        return;
    }
    holder->setProperty(property->propertyName().toUtf8().constData(), value);
}

void GDeskStyler::valueChanged(QtProperty *property, QString value)
{
    value_changed(holders[property], property, value);
    stream(qDebug(), style);
}

void GDeskStyler::valueChanged(QtProperty *property, QColor value)
{
    value_changed(holders[property], property, value);
    stream(qDebug(), style);
}

void GDeskStyler::valueChanged(QtProperty *property, int value)
{
    value_changed(holders[property], property, value);
    stream(qDebug(), style);
}

void GDeskStyler::on_actionExit_triggered()
{
    close();
}

void GDeskStyler::on_actionGenerate_triggered()
{
    style->generate();
}

void GDeskStyler::on_actionNew_triggered()
{
    filename = QString::null;

    ui->property_browser->clear();

    style = new GStyle();
    populate(style);
}

void GDeskStyler::on_actionSave_triggered()
{
    if (filename == QString::null)
    {
        on_actionSave_As_triggered();
    }
    else
    {
        save(filename);
    }
}

void GDeskStyler::on_actionSave_As_triggered()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    QString dir = settings.value("gss.dir", QCoreApplication::applicationDirPath()).toString();

    QString filename = QFileDialog::getSaveFileName(this, "Save style sheet", dir, "*.gss");

    if (filename != QString::null)
    {
        save(filename);

        QFileInfo info(filename);
        settings.setValue("gss.dir", info.absoluteDir().absolutePath());
    }

}

void GDeskStyler::save(QString filename)
{
    QFile gss_file(filename);
    gss_file.open(QIODevice::WriteOnly);
    QDataStream os(&gss_file);
    stream(os, style);
    gss_file.close();
}
