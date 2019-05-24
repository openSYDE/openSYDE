//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of system view toplogy (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSECONTEXTMENUMANAGER_H
#define C_SYVSECONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebTopologyBaseContextMenuManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeContextMenuManager :
   public C_SebTopologyBaseContextMenuManager
{
   Q_OBJECT

public:
   C_SyvSeContextMenuManager();
   virtual ~C_SyvSeContextMenuManager();

   virtual void HandleContextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event,
                                       const QList<QGraphicsItem *> & orc_SelectedItems, const bool & orq_ShowPaste);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowNodeInformation(QGraphicsItem * const opc_Item);
   void SigShowCanConfiguration(QGraphicsItem * const opc_Item);

private:
   //Avoid call
   C_SyvSeContextMenuManager(const C_SyvSeContextMenuManager &);
   C_SyvSeContextMenuManager & operator =(const C_SyvSeContextMenuManager &);

   void m_ShowNodeInformation(void);
   void m_ShowCanConfiguration(void);

   QAction * mpc_ActionShowNodeInformation;
   QAction * mpc_ActionShowCanConfiguration;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
