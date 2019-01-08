//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of base scene (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBBASECONTEXTMENUMANAGER_H
#define C_SEBBASECONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include <QGraphicsItem>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>

#include "C_OgeContextMenu.h"
#include "C_SebUtil.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebBaseContextMenuManager :
   public QObject
{
   Q_OBJECT

public:
   C_SebBaseContextMenuManager();
   virtual ~C_SebBaseContextMenuManager();

   virtual void HandleContextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event,
                                       const QList<QGraphicsItem *> & orc_SelectedItems, const bool & orq_ShowPaste);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigPaste(void);
   void SigCopy(void);
   void SigCut(void);
   void SigDelete(void);
   void SigSetupStyle(QGraphicsItem * const opc_Item);

   void SigBringToFront(void);
   void SigBringForward(void);
   void SigSendBackward(void);
   void SigSendToBack(void);
   void SigAlign(const QGraphicsItem * const opc_GuidelineItem,
                 const stw_opensyde_gui_logic::E_Alignment & ore_Alignment);

   void SigBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos);
   void SigRemoveBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos);

   void SigContextMenuClosed(void);

protected:
   stw_opensyde_gui_elements::C_OgeContextMenu mc_ContextMenu;
   stw_opensyde_gui_elements::C_OgeContextMenu mc_OrderObjectContextMenu;
   stw_opensyde_gui_elements::C_OgeContextMenu mc_AlignmentContextMenu;

   QGraphicsItem * mpc_ActiveItem;
   QPointF mc_ScenePos;

   QAction * mpc_ActionCopy;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionCopySeparator;
   QAction * mpc_ActionDelete;
   QAction * mpc_ActionDeleteSeparator;
   QAction * mpc_ActionSetupStyle;
   QAction * mpc_ActionBendLine;
   QAction * mpc_ActionRemoveBendLine;
   // Order objects actions
   QAction * mpc_ActionOrderObjects;
   QAction * mpc_ActionBringToFront;
   QAction * mpc_ActionBringForward;
   QAction * mpc_ActionSendBackward;
   QAction * mpc_ActionSendToBack;
   // Alignment actions
   QAction * mpc_ActionAlignment;
   QAction * mpc_ActionAlignLeft;
   QAction * mpc_ActionAlignHorizontalCenter;
   QAction * mpc_ActionAlignRight;
   QAction * mpc_ActionAlignTop;
   QAction * mpc_ActionAlignVerticalCenter;
   QAction * mpc_ActionAlignBottom;

   virtual void m_SetActionsInvisible(void);
   virtual bool m_ActivateSpecificActions(void) = 0;

   void m_ContextMenuClosed(void);
   void m_InsertBendLineActions(QAction * const opc_Action);

private:
   //Avoid call
   C_SebBaseContextMenuManager(const C_SebBaseContextMenuManager &);
   C_SebBaseContextMenuManager & operator =(const C_SebBaseContextMenuManager &);

   void m_Cut(void);
   void m_Copy(void);
   void m_Paste(void);
   void m_Delete(void);
   void m_SetupStyle(void);
   void m_BendLine(void);
   void m_RemoveBendLine(void);

   void m_AlignLeft(void);
   void m_AlignHorizontalCenter(void);
   void m_AlignRight(void);
   void m_AlignTop(void);
   void m_AlignVerticalCenter(void);
   void m_AlignBottom(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
