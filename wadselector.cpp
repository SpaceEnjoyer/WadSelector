#include <QDir>
#include <iostream>
#include <QTextStream>
#include <QMessageBox>
#include "wadselector.h"
#include "./ui_wadselector.h"

#define AUTOLOAD "[doom.Autoload]"
#define ENDREAD "[doom.id.Autoload]"

WadSelector::WadSelector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WadSelector)
{
    setWindowTitle("Wad Selector");
    ui->setupUi(this);
    defaults = defaults + "/.config/WadSelectorDefaults.txt";
    QFile defaultPaths(defaults);

    if(!defaultPaths.exists() || defaultPaths.size() == 0){
        defaultPaths.open(QFile::WriteOnly | QFile::Truncate);
        QTextStream f(&defaultPaths);
        pathToIni = pathToIni + "/.var/app/org.zdoom.GZDoom/.config/gzdoom/gzdoom.ini";
        pathToWads = pathToWads + "/wads";
        f << pathToWads << "\n" << pathToIni;
        defaultPaths.close();
    }
    else{
        defaultPaths.open(QFile::ReadOnly);
        QTextStream f(&defaultPaths);
        if(defaultPaths.isOpen()){
            pathToWads = f.readLine();
            pathToIni = f.readLine();
            defaultPaths.close();
        }
        else
            std::cerr<<"Could not open file\n";
    }

    ui->IniPathBox->setText(pathToIni);
    ui->WadPathBox->setText(pathToWads);
    grabWads();
}

WadSelector::~WadSelector()
{
    delete ui;
}

void WadSelector::grabWads(){
    ui->listWidget->clear();
    QDir directory(pathToWads);
    QStringList filters;
    filters << "*.WAD" << "*.wad" << "*.pk3" << "*.zip";
    directory.setNameFilters(filters);
    fileList = directory.entryInfoList();

    for (int i=0; i<fileList.size(); i++)
    {
        ui->listWidget->addItem(fileList.at(i).fileName());
        ui->listWidget->item(i)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if(readAddedWads(ui->listWidget->item(i)->text()))
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        else
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void WadSelector::addToIni(QString pathToIni, QString pathToWads){
    QFile iniFile(pathToIni);
    if(!iniFile.exists() || iniFile.size() == 0){
        QMessageBox::warning( this, tr("Warning"), tr("No ini file found") );
        return;
    }

    iniFile.open(QFile::ReadWrite);
    QTextStream in(&iniFile);
    QString temp="", read;

    if(iniFile.isOpen()){
        read = in.readLine();

        while(read != AUTOLOAD)
        {
            temp = temp + read + "\n";
            read = in.readLine();
        }
        temp = temp + AUTOLOAD + "\n\n";

        while(read != ENDREAD)
            read = in.readLine();       //Skip already loaded mods
    }
    else
        std::cerr<<"ERROR: File could not be opened\n";

    for(int i=0; i<selected.size(); i++){
        if(pathToWads[pathToWads.length()-1] != "/")    //In case the user omits '/'
            temp = temp + "Path=" + pathToWads + "/"+ selected[i] + "\n";   //no need to add '\' before spaces because gzdoom can parse whitespaces
        else
            temp = temp + "Path=" + pathToWads + selected[i] + "\n";
    }

    temp = "\n\n" + temp + ENDREAD + "\n";

    while(!in.atEnd()){
        read = in.readLine();
        temp = temp + read + "\n";
    }

    iniFile.close();
    iniFile.open(QFile::ReadWrite | QFile::Truncate);
    temp = temp.section('\n',2);        //Bruteforced it because everytime it finishes adding the mods it also adds 2 newlines on top of the file, annoying
    in << temp;

    iniFile.close();
}

bool WadSelector::readAddedWads(QString toBeChecked){
    QString line="";
    QFile ini(pathToIni);
    QTextStream in(&ini);
    ini.open(QFile::ReadOnly);

    if(ini.isOpen()){
        line = in.readLine();
        while(line != AUTOLOAD){
            line = in.readLine();
        }
        while(line != ENDREAD){
            line = in.readLine();
            if(line.section('/', -1) == toBeChecked)
                return true;
        }
        return false;
    }
    else{
        std::cerr<<"Could not open file\n";
        return false;
    }

}

void WadSelector::OnWadLoad()
{
    selected.clear();   //avoids duplicate lines in the ini if "load" button is clicked multiple times

    for(int i=0; i<fileList.size(); i++){
        if(ui->listWidget->item(i)->checkState() == 2){
            selected.push_back(ui->listWidget->item(i)->text());
        }
    }
    addToIni(pathToIni, pathToWads);

}

void WadSelector::OnWadSet()
{
    QString WadsDir = QFileDialog::getExistingDirectory(this, tr("Select Wads folder"), "/home", QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->WadPathBox->setText(WadsDir);
    pathToWads = ui->WadPathBox->text();

    QFile defaultPaths(defaults);
    defaultPaths.open(QFile::ReadOnly);
    QTextStream f(&defaultPaths);
    QString temp;
    temp = pathToWads + "\n";
    f.readLine();
    temp = temp + f.readLine();
    defaultPaths.close();
    defaultPaths.open(QFile::WriteOnly | QFile::Truncate);
    std::cout<<"temp: "<<temp.toStdString()<<"\n\n";
    f << temp;
    defaultPaths.close();

    grabWads();
}

void WadSelector::OnIniSet()
{
    QString iniFile = QFileDialog::getOpenFileName(this, "Select the gzdoom.ini file", "gzdoom.ini","Text files (*.ini)");

    if(iniFile.section('/', -1).toStdString() != "gzdoom.ini"){
        QMessageBox::warning( this, tr("Warning"), tr("Filename is not 'gzdoom.ini'.\nGZDoom will not read this!") );
    }

    ui->IniPathBox->setText(iniFile);
    pathToIni = ui->IniPathBox->text();

    QFile defaultPaths(defaults);
    defaultPaths.open(QFile::ReadOnly);
    QTextStream f(&defaultPaths);
    QString read;
    read = f.readLine() + "\n";
    read = read + iniFile;
    defaultPaths.close();
    defaultPaths.open(QFile::WriteOnly | QFile::Truncate);
    std::cout<<"read: "<<read.toStdString()<<"\n\n";
    f << read;
    defaultPaths.close();
}
