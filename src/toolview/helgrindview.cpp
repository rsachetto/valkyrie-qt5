/****************************************************************************
** HelgrindView implementation
**  - helgrind's personal window
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

#include "toolview/helgrindview.h"
#include "toolview/helgrind_logview.h"
#include "utils/vk_config.h"
#include "utils/vk_messages.h"
#include "utils/vk_utils.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMenuBar>
#include <QProcess>
#include <QToolBar>
#include <QVBoxLayout>


//TODO
//This is all simply a duplicate of MemcheckView.
//If further tools are unlikely to differ much, put it all in ToolView



/***************************************************************************/
/*!
  \class HelgrindView
  \brief This provides the HelgrindView user-interface

  This class is based on the abstract base class ToolView. Only one instance
  of the class is required, and is kept within the ToolViewStack (the central
  widget of MainWindow.

  The base class provides a tool-specific menu and toolbar, which is
  dynamically added/removed from MainWindow. The HelgrindView-specific actions
  for these menus are setup within this class.

  * TODO: more on link to tool_objects, when those are integrated...

  \sa ToolView, ToolViewStack, MainWindow
*/

/*!
    Constructs a HelgrindView with the given \a parent.
*/
HelgrindView::HelgrindView( QWidget* parent )
   : ToolView( parent, VGTOOL::ID_HELGRIND ), logview(0)
{
   setObjectName( QString::fromUtf8( "HelgrindView" ) );

   setupLayout();
   setupActions();
   setupToolBar();

   // enable | disable show*Item buttons
   connect( treeView, SIGNAL( itemSelectionChanged() ),
            this,       SLOT( updateItemActions() ) );

   // on collapsing a branch, reset currentItem to branch head.
   connect( treeView, SIGNAL( itemCollapsed( QTreeWidgetItem* ) ),
            this,       SLOT( itemCollapsed( QTreeWidgetItem* ) ) );

   // load items on-demand
   connect( treeView, SIGNAL( itemExpanded( QTreeWidgetItem* ) ),
            this,       SLOT( itemExpanded( QTreeWidgetItem* ) ) );

   // launch editor with src file loaded
   connect( treeView, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
            this,       SLOT( launchEditor( QTreeWidgetItem* ) ) );
}


/*!
    Destroys this widget, and frees any allocated resources.
*/
HelgrindView::~HelgrindView()
{
   if ( logview != 0 ) {
      delete logview;
      logview = 0;
   }
}


/*!
   Provide the tool-object access to our model, to fill it,
   but keep ownership ourselves: we know when we're done with it.

   Creates a clean log on each call.
   This should be called by the tool-object just before it intends
   to fill the log.
*/
VgLogView* HelgrindView::createVgLogView()
{
   if ( logview != 0 ) {
      delete logview;
   }

   logview = new HelgrindLogView( treeView );
   return logview;
}



/*!
    Setup the interface layout
*/
void HelgrindView::setupLayout()
{
   QVBoxLayout* vLayout = new QVBoxLayout( this );
   vLayout->setMargin(0);
   
   treeView = new QTreeWidget( this );
   treeView->setObjectName( QString::fromUtf8( "treeview_Helgrind" ) );
   treeView->setHeaderHidden( true );
   treeView->setRootIsDecorated( false );
   vLayout->addWidget( treeView );
}


/*!
    Setup the tool-specific actions (later added to the menu / toolbar)
*/
void HelgrindView::setupActions()
{
   // ------------------------------------------------------------
   // Define actions
   act_OpenClose_item = new QAction( this );
   act_OpenClose_item->setObjectName( QString::fromUtf8( "act_OpenClose_item" ) );
   QIcon icon_opencloseitem;
   icon_opencloseitem.addPixmap( QPixmap( QString::fromUtf8( ":/vk_icons/icons/item_open.png" ) ) );
   act_OpenClose_item->setIcon( icon_opencloseitem );
   act_OpenClose_item->setIconVisibleInMenu( true );
   connect( act_OpenClose_item, SIGNAL( triggered() ),
            this,                   SLOT( opencloseOneItem() ) );

   act_OpenClose_all = new QAction( this );
   act_OpenClose_all->setObjectName( QString::fromUtf8( "act_OpenClose_all" ) );
   QIcon icon_opencloseall;
   icon_opencloseall.addPixmap( QPixmap( QString::fromUtf8( ":/vk_icons/icons/tree_open.png" ) ) );
   act_OpenClose_all->setIcon( icon_opencloseall );
   act_OpenClose_all->setIconVisibleInMenu( true );
   connect( act_OpenClose_all, SIGNAL( triggered() ),
            this,                  SLOT( opencloseAllItems() ) );

   act_ShowSrcPaths = new QAction( this );
   act_ShowSrcPaths->setObjectName( QString::fromUtf8( "act_ShowSrcPaths" ) );
   QIcon icon_showsrcpaths;
   icon_showsrcpaths.addPixmap( QPixmap( QString::fromUtf8( ":/vk_icons/icons/text_more.png" ) ) );
   act_ShowSrcPaths->setIcon( icon_showsrcpaths );
   act_ShowSrcPaths->setIconVisibleInMenu( true );
   connect( act_ShowSrcPaths, SIGNAL( triggered() ), this, SLOT( showSrcPath() ) );

   act_OpenLog = new QAction( this );
   act_OpenLog->setObjectName( QString::fromUtf8( "act_OpenLog" ) );
   QIcon icon_openlog;
   icon_openlog.addPixmap( QPixmap( QString::fromUtf8( ":/vk_icons/icons/folder_green.png" ) ) );
   act_OpenLog->setIcon( icon_openlog );
   act_OpenLog->setIconVisibleInMenu( true );
   connect( act_OpenLog, SIGNAL( triggered() ), this, SLOT( openLogFile() ) );

   act_SaveLog = new QAction( this );
   act_SaveLog->setObjectName( QString::fromUtf8( "act_SaveLog" ) );
   QIcon icon_savelog;
   icon_savelog.addPixmap( QPixmap( QString::fromUtf8( ":/vk_icons/icons/filesaveas.png" ) ) );
   act_SaveLog->setIcon( icon_savelog );
   act_SaveLog->setIconVisibleInMenu( true );
   connect( act_SaveLog, SIGNAL( triggered() ), this, SIGNAL( saveLogFile() ) );

   // ------------------------------------------------------------
   // initialise actions (enable / disable)
   setState( false );

   // ------------------------------------------------------------
   // Text
   act_OpenClose_item->setText(    tr( "Open/Close item" ) );
   act_OpenClose_item->setToolTip( tr( "Open/Close currently selected item" ) );
   act_OpenClose_all->setText(     tr( "Open/Close all" ) );
   act_OpenClose_all->setToolTip(  tr( "Open/Close all Valgrind::ERROR items" ) );
   act_ShowSrcPaths->setText(      tr( "Display simple" ) );
   act_ShowSrcPaths->setToolTip(   tr( "Display complex / simplified view" ) );

   act_OpenLog->setText(    tr( "Open Log" ) );
   act_OpenLog->setToolTip( tr( "Open XML log" ) );
   act_SaveLog->setText(    tr( "Save Log" ) );
   act_SaveLog->setToolTip( tr( "Save Valgrind output to an XML log" ) );
}


/*!
    Setup the tool-specific toolbar and menu.
    These are dynamically added/removed (under control of the ToolViewStack)
    to MainWindow.
*/
void HelgrindView::setupToolBar()
{
   // ------------------------------------------------------------
   // ToolBar (created in base class)
   toolToolBar->setObjectName( QString::fromUtf8( "helgrindToolBar" ) );
   toolToolBar->addAction( act_OpenClose_item );
   toolToolBar->addAction( act_OpenClose_all );
   toolToolBar->addAction( act_ShowSrcPaths );
   toolToolBar->addAction( act_OpenLog );
   toolToolBar->addAction( act_SaveLog );

   // ------------------------------------------------------------
   // Menu (created in base class)
   toolMenu->setObjectName( QString::fromUtf8( "helgrindMenu" ) );
   toolMenu->setTitle( tr( "Helgrind" ) );

   toolMenu->addAction( act_OpenClose_item );
   toolMenu->addAction( act_OpenClose_all );
   toolMenu->addAction( act_ShowSrcPaths );
   toolMenu->addAction( act_OpenLog );
   toolMenu->addAction( act_SaveLog );
}


/*!
  Called by tool object
   - set state for buttons; set cursor state
*/
void HelgrindView::setState( bool run )
{
   act_OpenLog->setEnabled( !run );  // just turn off while running

   if ( run ) {
      // turn off while running...
      act_OpenClose_item->setEnabled( false );
      act_OpenClose_all->setEnabled( false );
      act_ShowSrcPaths->setEnabled( false );
      act_SaveLog->setEnabled( false );

      this->setCursor( QCursor( Qt::WaitCursor ) );
      treeView->clear();
   }
   else {
      unsetCursor();

      // ... turn on again only if they can be used
      bool tree_empty = ( treeView->topLevelItemCount() == 0 );
      act_OpenClose_item->setEnabled( false );       // can't enable before item clicked
      act_OpenClose_all->setEnabled( !tree_empty );  // enable only if sthng in tree
      act_ShowSrcPaths->setEnabled( !tree_empty );   // enable only if sthng in tree
      act_SaveLog->setEnabled( !tree_empty );
   }
}


/*!
    Launches an editor for the given \a item.
    Checks if the itemType() is of type SRC_CODE,
    and if the referenced file isReadable|isWriteable.
    If these checks are passed, the (option-configurable) editor
    is launched to open the source code file at the correct
    code line.

    TODO: what if fails tests: user message?
*/
void HelgrindView::launchEditor( QTreeWidgetItem* item )
{
   //vkDebug( "HelgrindView::launchEditor( %s )", qPrintable( item->text( 0 ) ) );

   VgOutputItem* vgItemCurr = (VgOutputItem*)item;
   if ( !vgItemCurr ||
        !vgItemCurr->parent() ) {
      return;
   }

   // only interested in SrcItem items (== LINE type)
   if ( vgItemCurr->elemType() != VG_ELEM::LINE ||
        vgItemCurr->parent()->elemType() != VG_ELEM::FRAME ) {
      return;
   }

   // nothing to do if not even readable :-(
   // in principle, if a src item is visible, it should be readable,
   // but you never know...
   if ( !vgItemCurr->getIsReadable() ) {
      vkError( this, "Editor Launch", "<p>Source file not readable.</p>" );
      return;
   }

   // check editor is set
   QString editor = vkCfgProj->value( "valkyrie/src-editor" ).toString();
   if ( editor.isEmpty() ) {
      vkError( this, "Editor Launch",
               "<p>Source editor not set.<br>"
               "This can be set via Edit->Options->Valkyrie.</p>" );
      return;
   }

   // get path,line for this frame
   FrameItem* frame = (FrameItem*)vgItemCurr->parent();

   QDomNodeList frame_details = frame->getElement().childNodes();
   vk_assert( frame_details.count() >= 1 );   // only ip guaranteed
   QDomElement dir    = frame_details.item( 3 ).toElement();
   QDomElement srcloc = frame_details.item( 4 ).toElement();
   QDomElement line   = frame_details.item( 5 ).toElement();

   if ( dir.isNull() || srcloc.isNull() ) {
      VK_DEBUG( "HelgrindView::launchEditor(): Not enough path information." );
      vkError( this, "Editor Launch", "<p>Not enough path information.</p>" );
      return;
   }

   QString path( dir.text() + '/' + srcloc.text() );
   vk_assert( !path.isEmpty() );

   // setup args to editor
   QStringList args = editor.split( " " );
   QString  program = args.at( 0 );
   args = args.mid( 1 );

   if ( line.isNull() ) {
      // remove any arg with "%n" in it
      QStringList lineargs = args.filter(".*%n.*");
      QStringList::iterator it = lineargs.begin();
      for (; it != lineargs.end(); ++it ) {
         args.removeAll( *it );
      }
   } else {
      args.replaceInStrings( "%n", line.text() );
   }
   args << path;

   // launch editor in a new process, and detach from it.
   // process will continue to live, even if Valkrie exits.
   if ( ! QProcess::startDetached( program, args ) ) {
      VK_DEBUG("HelgrindView::launchEditor(): Failed to launch editor: %s",
               qPrintable( args.join(" ") ) );
      vkError( this, "Editor Launch",
               "<p>Failed to launch editor:<br>%s %s</p>",
               qPrintable( program ),
               qPrintable( args.join("<br>") ) );
   }
}


/*!
    Shows/Hides the file paths for all frames under current item.
*/
void HelgrindView::showSrcPath()
{
   //vkDebug( "HelgrindView::showSrcPath()" );

   if ( treeView->topLevelItemCount() == 0 ) {
      return;
   }
   VgOutputItem* vgItemTop = (VgOutputItem*)treeView->topLevelItem( 0 );

   VgOutputItem* vgItem = (VgOutputItem*)treeView->currentItem();
   if ( !vgItem ) {
      vgItem = vgItemTop;
   }

   // if we're top dog, show full src path for all _open_ error items.
   // Note: not supporting UNshow for all. Don't think worth the effort.
   if ( vgItem == vgItemTop ) {
      for ( int i=0; i<vgItem->childCount(); ++i ) {
         VgOutputItem* child = (VgOutputItem*)vgItem->child( i );
         if ( child->isExpanded() &&
              child->elemType() == VG_ELEM::ERROR ) {
            ErrorItem* error = (ErrorItem*)vgItem->child( i );
            error->showFullSrcPath( true );
         }
      }
      return;
   }

   // else, we're not top level item...
   // in case we're hanging out on a branch somewhere,
   // crawl up the branch until we're a first-child item
   vk_assert( vgItem->parent() != 0 );
   vk_assert( vgItem != vgItemTop );
   while ( vgItem->parent() != vgItemTop ) {
      vgItem = vgItem->parent();
   }

   // if we're an _open_ ERROR-item, then show src path for this item only.
   // Toggling of show-full-src-paths supported for this case.
   if ( vgItem->isExpanded() &&
        vgItem->elemType() == VG_ELEM::ERROR ) {
      ErrorItem* error = (ErrorItem*)vgItem;
      error->showFullSrcPath( !error->isFullSrcPathShown() );
   }
}


/*!
    Opens all error items, including their children.
    Ignores non-error items (status, preample, etc).
*/
void HelgrindView::opencloseAllItems()
{
   //vkDebug( "HelgrindView::opencloseAllItems()" );

   if ( treeView->topLevelItemCount() == 0 ) {
      // empty tree.
      return;
   }

   VgOutputItem* vgItemTop = (VgOutputItem*)treeView->topLevelItem( 0 );
   if ( !vgItemTop || vgItemTop->childCount() == 0 ) {
      vkPrintErr( "Error: listview not populated. This shouldn't happen!" );
      return;
   }

   // iterate over the first-child items
   // check item->isOpen, start from first error, ignore suppcounts
   bool anItemIsOpen = false;
   int idxItemERR = -1;
   for ( int i=0; i<vgItemTop->childCount(); ++i ) {
      VgOutputItem* child = (VgOutputItem*)vgItemTop->child( i );

      // find the first ERROR element
      if ( (idxItemERR == -1) &&
           child->elemType() == VG_ELEM::ERROR ) {
         idxItemERR = i;
      }

      // and check all elements from then on for isExpanded()
      if ( idxItemERR != -1 ) {
         // skip suppressions
         if ( child->elemType() == VG_ELEM::SUPPCOUNTS ) {
            continue;
         }
         if ( child->isExpanded() ) {
            anItemIsOpen = true;
            break;
         }
      }
   }
   if ( idxItemERR == -1 ) {
      // first ERROR element not found - perhaps there were no errors!
      vkDebug( "No VG_ELEM::ERROR found." );
      return;
   }

   // iterate over the same items, opening or collapsing all.
   // note: only opening/collapsing first-child level, not all levels.
   for ( int i=idxItemERR; i<vgItemTop->childCount(); ++i ) {
      VgOutputItem* child = (VgOutputItem*)vgItemTop->child( i );
      // skip suppressions
      if ( child->elemType() == VG_ELEM::SUPPCOUNTS ) {
         continue;
      }
      child->setExpanded( !anItemIsOpen );
   }


   if ( anItemIsOpen ) {
      // We've collapsed all ERROR branches.
      // Collapsing a branch sets currentItem to branch head
      // - giving currentItem == last branch to be collapsed.
      // Too much work to figure out if we were previously
      // inside a now collapsed branch. Just reset to top.
      treeView->setCurrentItem( vgItemTop );
   }
}


/*!
    Opens/closes the current tree-item.

    When opening, all the children of the item are also opened.
    When closing, only the selected item is closed.
*/
void HelgrindView::opencloseOneItem()
{
   //vkDebug( "HelgrindView::opencloseOneItem():" );
   
   QTreeWidgetItem* item = treeView->currentItem();
   if ( item == 0 )
      return;

   item->setExpanded( !item->isExpanded() );
}


/*!
  void HelgrindView::itemExpanded( QTreeWidgetItem* item )

  Supports on-demand loading our VgOutputItems from our underlying model (VgElements)
  Have to catch the treeView::itemExpanded() signal and pass it on to our
  implementation of QTreeWidgetItem (VgOutputItem).

  This should be in VgOutputItem class, but QTreeWidgetItem no longer
  provides a virtual setExpanded() method :-(
*/
void HelgrindView::itemExpanded( QTreeWidgetItem* item )
{
   //vkDebug( "HelgrindView::itemExpanded():" );
   ((VgOutputItem*)item)->openChildren();
}


/*!
  if we collapse a branch, set current item to branch head
*/
void HelgrindView::itemCollapsed( QTreeWidgetItem* item )
{
   //vkDebug( "HelgrindView::itemCollapsed():" );

   if ( item != treeView->currentItem() ) {
      // this should be a slot. grr!
      treeView->setCurrentItem( item );
   }
}


/*!
    Updates actions dependent on currently selected item.
*/
void HelgrindView::updateItemActions()
{
   //vkDebug( "HelgrindView::updateItemActions():" );

   QTreeWidgetItem* item = treeView->currentItem();
   if ( !item ) {
      act_OpenClose_item->setEnabled( false );
   }
   else {
      // item ok: contract / expand it
      VgOutputItem* vgItem = (VgOutputItem*)item;
      act_OpenClose_item->setEnabled( vgItem->getIsExpandable() );
   }
}
