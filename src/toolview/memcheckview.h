/****************************************************************************
** MemcheckView definition
**  - memcheck's personal window
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

#ifndef __MEMCHECKVIEW_H
#define __MEMCHECKVIEW_H

#include "toolview/toolview.h"
#include "toolview/vglogview.h"
#include "toolview/logviewfilter_mc.h"

#include <QMenu>
#include <QTreeWidget>
#include <QToolButton>


// ============================================================
class MemcheckView : public ToolView
{
   Q_OBJECT
public:
   MemcheckView( QWidget* parent );
   ~MemcheckView();
   
   VgLogView* createVgLogView();
   
public slots:
   virtual void setState( bool run );
   
private:
   void setupLayout();
   void setupActions();
   void setupToolBar();
   
private slots:
   void opencloseAllItems();
   void opencloseOneItem();
   void showSrcPath();
   void launchEditor( QTreeWidgetItem* item );
   void itemExpanded( QTreeWidgetItem* item );
   void itemCollapsed( QTreeWidgetItem* item );
   void popupMenu( const QPoint& pos );
   void updateItemActions();

private:
   QAction* act_OpenClose_all;
   QAction* act_OpenClose_item;
   QAction* act_ShowSrcPaths;
   QAction* act_OpenLog;
   QAction* act_SaveLog;
   QAction* act_enableFilter;
   
   QTreeWidget* treeView;
   VgLogView*   logview;
   
   LogViewFilterMC* logviewFilter;
};

#endif // __MEMCHECKVIEW_H
