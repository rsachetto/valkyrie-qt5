/****************************************************************************
** URL strings definition
**  - placed in one file in an endeavour to minimise
**    what is probably going to be a maintenance nightmare.
** --------------------------------------------------------------------------
**
** Copyright (C) 2000-2011, OpenWorks LLP. All rights reserved.
** <info@open-works.co.uk>
**
** This file is part of Valkyrie, a front-end for Valgrind.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef __VK_HELP_URLS_H
#define __VK_HELP_URLS_H


// not every flag/option has context-sensitive help
extern const char* urlNone;


// ============================================================
// Valkyrie
namespace urlValkyrie
{
// Mainwindow
extern const char* menuBar;
extern const char* fileMenu;
extern const char* toolMenu;
extern const char* optionsMenu;
extern const char* runButton;
extern const char* stopButton;
extern const char* helpMenu;
extern const char* statusMsg;
extern const char* flagsWidget;

// options dialog
extern const char* optsDlg;

// valkyrie's options page
extern const char* optsPage;
extern const char* toolTips;
extern const char* toolLabels;
extern const char* browser;
extern const char* logDir;
extern const char* workingDir;
extern const char* projectFile;
extern const char* userFontGen;
extern const char* userFontTool;
extern const char* palette;
extern const char* srcLines;
extern const char* srcEditor;
extern const char* binary;
extern const char* binFlags;
extern const char* vgDir;

// valgrind's options page
extern const char* coreTab;
extern const char* errorTab;
extern const char* suppsTab;

// MemcheckView tool buttons
extern const char* openAllButton;
extern const char* openOneButton;
extern const char* srcPathButton;
extern const char* loadLogButton;
extern const char* mrgLogButton;
extern const char* saveLogButton;
extern const char* suppEdButton;
}


// ============================================================
// Valgrind core
namespace urlVgCore
{
// valgrind's options page: tab Core
extern const char* mainTool;
extern const char* verbosity;
extern const char* xmlOutput;
extern const char* xmlComment;
extern const char* traceChild;
extern const char* silentChild;
extern const char* trackFds;
extern const char* timeStamp;
extern const char* freeGlibc;
extern const char* showEmWarns;
extern const char* smcSupport;
extern const char* simHints;
extern const char* kernelVariant;

// valgrind's options page: tab Error Reporting
extern const char* genSuppressions;
extern const char* autoDemangle;
extern const char* errorLimit;
extern const char* stackTraces;
extern const char* numCallers;
extern const char* maxSFrames;
extern const char* startDebugger;
extern const char* whichDebugger;
extern const char* inputFd;
extern const char* logToFd;
extern const char* logToFile;
extern const char* logToSocket;

// only used by Memcheck and Massif
extern const char* Alignment;
}


// ============================================================
// Memcheck
namespace urlMemcheck
{
extern const char* optsMC;
extern const char* Partial;
extern const char* Freelist;
extern const char* Leakcheck;
extern const char* Leakres;
extern const char* Showreach;

//extern const char* UndefVal;
extern const char* TrackOri;
extern const char* gcc296;
}


// ============================================================
// Cachegrind
namespace urlCachegrind
{
extern const char* optsCG;
extern const char* Cacheopts;
extern const char* Pid;
extern const char* Sort;
extern const char* Show;
extern const char* Threshold;
extern const char* Auto;
extern const char* Context;
extern const char* Include;
}


// ============================================================
// Massif
namespace urlMassif
{
extern const char* optsMS;
extern const char* optsMassif;
extern const char* Heap;
extern const char* HeapAdmin;
extern const char* Stacks;
extern const char* Depth;
extern const char* AllocFn;
extern const char* Format;
}


#endif
