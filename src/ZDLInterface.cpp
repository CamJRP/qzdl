/*
 * This file is part of qZDL
 * Copyright (C) 2007-2011  Cody Harris
 * 
 * qZDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include "ZDLConfigurationManager.h"
#include "ZDLAdvancedMultiplayerDialog.h"
#include "ZDLAboutDialog.h"

#include "ZDLMultiPane.h"
#include "ZDLInterface.h"
#include "ZDLInfoBar.h"
#include "ZDLMainWindow.h"
#include "ZDLFilePane.h"
#include "settingPane.h"
#include "ZDLQSplitter.h"
#include "ZDMFlagPicker.h"

#include "aup.xpm"
#include "adown.xpm"

extern ZDLMainWindow *mw;
extern QApplication *qapp;

ZDLInterface::ZDLInterface(QWidget *parent):ZQWidget(parent){
	ZDLConfigurationManager::setInterface(this);
	
	box = new QVBoxLayout(this);
	
	
	QLayout *tpane = getTopPane();
	QLayout *bpane = getBottomPane();
	
	mpane = NULL;
	setContentsMargins(0,0,0,0);
	layout()->setContentsMargins(0,0,0,0);
	box->setSpacing(0);
	ZDLInfoBar *zib = new ZDLInfoBar(this);
	box->addWidget(zib);
	
	ZDLConfigurationManager::setInfobar(zib);
	box->addLayout(tpane);
	box->addLayout(bpane);
	
}

QLayout *ZDLInterface::getTopPane(){
	QHBoxLayout *box = new QHBoxLayout();

	ZDLQSplitter *split = new ZDLQSplitter(this);
	QSplitter *rsplit = split->getSplit();
	

	ZDLFilePane *fpane = new ZDLFilePane(rsplit);
	settingPane *spane = new settingPane(rsplit);
	
	split->addChild(fpane);
	split->addChild(spane);
	box->setSpacing(2);
	box->addWidget(rsplit);
	return box;
}

QLayout *ZDLInterface::getBottomPane(){
	QVBoxLayout *box = new QVBoxLayout();
	QLabel *ecla = new QLabel("Extra command line arguments",this);
	extraArgs = new QLineEdit(this);
	QLayout *pan = getButtonPane();
	layout()->setContentsMargins(0,0,0,0);
	setContentsMargins(0,0,0,0);
	box->setSpacing(2);
	box->addWidget(ecla);
	box->addWidget(extraArgs);
	box->addLayout(pan);
	return box;
}

void ZDLInterface::exitzdl(){
	mw->close();
}

QLayout *ZDLInterface::getButtonPane(){
	QHBoxLayout *box = new QHBoxLayout();

	QPushButton *btnExit = new QPushButton("Exit", this);
	btnZDL = new QPushButton("ZDL", this);
	QPushButton *btnMSet = new QPushButton("Multi Settings", this);
	btnEpr = new QPushButton(this);
	QPushButton *btnLaunch = new QPushButton("Launch", this);

	QMenu *context = new QMenu(btnZDL);
	QMenu *actions = new QMenu("Actions",context);
	
	QAction *showCommandline = actions->addAction("Show Command Line");
	actions->addAction("Clear PWAD list");
	actions->addAction("Clear all fields");
	//QAction *newDMFlagger = actions->addAction("New DMFlag picker");
	
	context->addMenu(actions);
	context->addSeparator();
	QAction *loadZdlFileAction = context->addAction("Load .zdl");
	QAction *saveZdlFileAction = context->addAction("Save .zdl");
	context->addSeparator();
	QAction *loadAction = context->addAction("Load .ini");
	QAction *saveAction = context->addAction("Save .ini");
	context->addSeparator();
	QAction *aboutAction = context->addAction("About");
	
	connect(loadAction, SIGNAL(triggered()), this, SLOT(loadConfigFile()));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveConfigFile()));
	connect(loadZdlFileAction, SIGNAL(triggered()), this, SLOT(loadZdlFile()));
	connect(saveZdlFileAction, SIGNAL(triggered()), this, SLOT(saveZdlFile()));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutClick()));
	connect(showCommandline, SIGNAL(triggered()),this,SLOT(showCommandline()));
	//connect(newDMFlagger, SIGNAL(triggered()),this,SLOT(showNewDMFlagger()));
	connect(btnExit, SIGNAL(clicked()), this, SLOT(exitzdl()));
	
	btnZDL->setMenu(context);
	
	int minBtnWidth = 50;

	btnExit->setMinimumWidth(minBtnWidth-20);
	btnZDL->setMinimumWidth(minBtnWidth-20);
	btnMSet->setMinimumWidth(minBtnWidth+40);
	btnEpr->setMinimumWidth(20);
	btnLaunch->setMinimumWidth(minBtnWidth);

	connect(btnLaunch, SIGNAL( clicked() ), this, SLOT(launch()));

	setContentsMargins(0,0,0,0);
	layout()->setContentsMargins(0,0,0,0);
	box->setSpacing(2);
	box->addWidget(btnExit);
	box->addWidget(btnZDL);
	box->addWidget(btnMSet);
	box->addWidget(btnEpr);
	box->addWidget(btnLaunch);
	connect(btnEpr, SIGNAL(clicked()), this, SLOT(mclick()));
	
	connect(btnMSet, SIGNAL(clicked()), this, SLOT(ampclick()));
	return box;
}

void ZDLInterface::showNewDMFlagger(){
	ZDMFlagPicker dialog(this);
	dialog.exec();
}

void ZDLInterface::launch(){
	mw->launch();
}

void ZDLInterface::buttonPaneNewConfig(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	ZDLSection *section = zconf->getSection("zdl.save");
	if (section){
		QVector<ZDLLine*> vctr;
		section->getRegex("^dlgmode$", vctr);
		for(int i = 0; i < vctr.size(); i++){
			if (vctr[i]->getValue().compare("open",Qt::CaseInsensitive) == 0){
				btnEpr->setIcon(QPixmap(adown));
			}else{
				btnEpr->setIcon(QPixmap(aup));
			}
		}
		if (vctr.size() < 1){
			btnEpr->setIcon(QPixmap(aup));
		}
	}else{
		btnEpr->setIcon(QPixmap(aup));
	}
	
}

void ZDLInterface::mclick(){
	writeConfig();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	int stat;
	if(zconf->hasValue("zdl.save","dlgmode")){
		QString txt = zconf->getValue("zdl.save","dlgmode",&stat);
		if(txt == "closed"){
			btnEpr->setIcon(QPixmap(adown));
			zconf->setValue("zdl.save", "dlgmode", "open");
		}else{
			zconf->setValue("zdl.save", "dlgmode", "closed");
			btnEpr->setIcon(QPixmap(aup));
		}
	}else{
		btnEpr->setIcon(QPixmap(adown));
		zconf->setValue("zdl.save", "dlgmode", "open");
	}
	startRead();
}

void ZDLInterface::ampclick(){
	ZDLAdvancedMultiplayerDialog *zampd = new ZDLAdvancedMultiplayerDialog(this);
	zampd->exec();
	delete zampd;
}

void ZDLInterface::sendSignals(){
	rebuild();
	emit buildParent(this);
	emit buildChildren(this);
}

void ZDLInterface::saveConfigFile(){
	sendSignals();
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	QStringList filters;
	filters << "ini (*.ini)"
			<< "ini Files (*.ini)"
			<< "Any files (*)";
	QFileDialog dialog(this,"Save");
	dialog.setFilters(filters);
	QStringList fileNames;
	if(dialog.exec()){
		fileNames = dialog.selectedFiles();
		for(int i = 0; i < fileNames.size(); i++){
			ZDLConfigurationManager::setConfigFileName(fileNames[i]);
			zconf->writeINI(fileNames[i]);
		}
		mw->startRead();
	}
	
}

void ZDLInterface::loadConfigFile(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	QStringList filters;
	filters << "ini (*.ini)"
			<< "ini Files (*.ini)"
			<< "Any files (*)";
	QFileDialog dialog(this);
	dialog.setFilters(filters);
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList fileNames;
	if(dialog.exec()){
		fileNames = dialog.selectedFiles();
		for(int i = 0; i < fileNames.size(); i++){
			delete zconf;
			ZDLConf* tconf = new ZDLConf();
			ZDLConfigurationManager::setConfigFileName(fileNames[i]);
			tconf->readINI(fileNames[i]);
			ZDLConfigurationManager::setActiveConfiguration(tconf);
			
		}
		mw->startRead();
	}
	
}

void ZDLInterface::loadZdlFile(){
	QStringList filters;
	filters << "ZDL (*.zdl)" << "Any files (*)";
	QFileDialog dialog(this,"Load");
	dialog.setFilters(filters);
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList fileNames;
	if(dialog.exec()){
		fileNames = dialog.selectedFiles();
		if(fileNames.size() == 1){
			ZDLConf *current = ZDLConfigurationManager::getActiveConfiguration();
			for(int i = 0; i < current->sections.size(); i++){
				if(current->sections[i]->getName().compare("zdl.save") == 0){
					ZDLSection *section = current->sections[i];
					current->sections.remove(i);
					delete section;
					break;
				}
			}
			QString fileName = fileNames[0];
			ZDLConf *newConf = new ZDLConf();
			newConf->readINI(fileName);
			for(int i = 0; i < newConf->sections.size(); i++){
				if(newConf->sections[i]->getName().compare("zdl.save") == 0){
					ZDLSection *section = newConf->sections[i];
					current->addSection(section->clone());
					break;
				}
			}
			delete newConf;
			mw->startRead();
		}else{
			QMessageBox::critical(this,ZDL_ENGINE_NAME " Load .ZDL Error", "You must select one and only one .zdl file at a time to load",QMessageBox::Ok,QMessageBox::Ok);
		}
	}
}

void ZDLInterface::saveZdlFile(){
	sendSignals();
	QStringList filters;
	filters << "ZDL (*.zdl)" << "Any files (*)";
	QFileDialog dialog(this,"Save");
	dialog.setFilters(filters);
	QStringList fileNames;
	if(dialog.exec()){
		ZDLConf *current = ZDLConfigurationManager::getActiveConfiguration();
		ZDLConf *copy = new ZDLConf();
		for(int i = 0; i < current->sections.size(); i++){
			if(current->sections[i]->getName().compare("zdl.save") == 0){
				copy->addSection(current->sections[i]->clone());
				fileNames = dialog.selectedFiles();
				for(int i = 0; i < fileNames.size(); i++){
					QString fileName = fileNames[i];
					if(!fileName.contains(".")){
						fileName = fileName + QString(".zdl");
					}
					copy->writeINI(fileNames[i]);
				}
			}
		}
	}
	
}

void ZDLInterface::aboutClick(){
	ZDLAboutDialog zad(this);
	zad.exec();
}

void ZDLInterface::showCommandline(){
	writeConfig();
	
	QString exec = mw->getExecutable();
	if (exec.length() < 1){
		QMessageBox::critical(this, ZDL_ENGINE_NAME, "Please select a source port");
		return;
	}
	QStringList args = mw->getArguments();
	if (args.join("").length() < 1){
		return;
	}

	if(exec.contains("\\")){
		exec.replace("\\","/");
	}
	
	//Find the executable
	QStringList executablePath = exec.split("/");

	//Remove the last item, which will be the .exe
	executablePath.removeLast();

	//Re-create the string
	QString workingDirectory = executablePath.join("/");

	//Resolve the path to an absolute directory
	QDir cwd(workingDirectory);
	workingDirectory = cwd.absolutePath();
	// Turns on launch confirmation
	QMessageBox::StandardButton btnrc = QMessageBox::question(this, "Would you like to continue?","Executable: "+exec+"\n\nArguments: "+args.join(" ")+"\n\nWorking Directory: "+workingDirectory, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if(btnrc == QMessageBox::Yes){
		mw->launch();
		return;
	}
}

void ZDLInterface::rebuild(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(extraArgs->text().length() > 0){
		zconf->setValue("zdl.save", "extra", extraArgs->text().toStdString().c_str());
		
	}else{
		zconf->deleteValue("zdl.save", "extra");
	}
}

void ZDLInterface::bottomPaneNewConfig(){
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	if(zconf->hasValue("zdl.save", "extra")){
		int stat;
		QString rc = zconf->getValue("zdl.save", "extra", &stat);
		if(rc.length() > 0){
			extraArgs->setText(rc);
		}
	}else{
		extraArgs->setText("");
	}
}

//Called when there's a change to the configuration that we need to look at.
//The button changed the configuration, and then notifies us that we need
//to look at the configuration to see what we need to do.
void ZDLInterface::newConfig(){
	buttonPaneNewConfig();
	bottomPaneNewConfig();
	//Grab our configuration
	ZDLConf *zconf = ZDLConfigurationManager::getActiveConfiguration();
	//Grab our section in the configuration
	ZDLSection *section = zconf->getSection("zdl.save");
	//Do we have it?
	if (section){
		QVector<ZDLLine*> vctr;
		//Search for our string
		section->getRegex("^dlgmode$", vctr);
		for(int i = 0; i < vctr.size(); i++){
			//Are we open?
			if (vctr[i]->getValue().compare("open", Qt::CaseInsensitive) == 0){
				// If the multiplayer pane has not been created, create one
				if(mpane == NULL){
					mpane = new ZDLMultiPane(this);
					mpane->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum ));
				}
				box->addWidget(mpane);
				mpane->setVisible(true);
			//No?
			}else{
				if(mpane){
					box->removeWidget(mpane);
					mpane->setVisible(false);
					mpane->hide();
				}
			}
		}
		//Do we have the section, but not the key?
		if (vctr.size() < 1){
			if(mpane){
				box->removeWidget(mpane);
				mpane->setVisible(false);
			}
		}
	//Do we not even have the section?
	}else{
		if(mpane){
			box->removeWidget(mpane);
		}
	}
	this->update();
}



void ZDLInterface::startRead(){
	emit readChildren(this);
	newConfig();
}

void ZDLInterface::writeConfig(){
	rebuild();
	emit buildChildren(this);
}
