//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view setup (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSESCENE_H
#define C_SYVSESCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>
#include "stwtypes.h"

#include "C_SyvTopologyBaseScene.h"
#include "C_SebUnoTopBaseManager.h"
#include "C_SyvSeContextMenuManager.h"

#include "C_GiNode.h"
#include "C_GiSvPcBusConnector.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeScene :
   public C_SyvTopologyBaseScene
{
   Q_OBJECT

public:
   C_SyvSeScene(const stw_types::uint32 ou32_ViewIndex = 0, QObject * const opc_Parent = NULL);
   virtual ~C_SyvSeScene(void);

   void PrepareToClose(void);

   void SetEditMode(const bool oq_Active);
   void Save(void) const;
   void ReloadViewData(void);
   void ClearUndoStack(void);

   void SetAllNodesConnected(const bool oq_Connected);

   virtual bool IsAnyItemAddable(void) const override;
   virtual bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsSceneRubberBandAvailable(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorCheck(void) const;
   void SigConnectionChange(void) const;

protected:
   virtual C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   virtual stw_opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   virtual C_GiNode * m_CreateNode(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                   const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_Height,
                                   QGraphicsItem * const opc_Parent) override;

   virtual void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;
   virtual void m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem) override;
   virtual void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;

   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvSeScene(const C_SyvSeScene &);
   C_SyvSeScene & operator =(const C_SyvSeScene &);

   void m_Edit(QGraphicsItem * const opc_GraphicsItem) const;
   void m_SelectionChanged(void);
   void m_OnPcBusConnectionStartConnect(const C_GiSvPcBusConnector::E_ConnectState & ore_ConnectState,
                                        QGraphicsItem * const opc_Item,
                                        const stw_opensyde_core::C_OSCSystemBus::E_Type * const opc_Type,
                                        C_GiLiBusConnectorBase * const opc_Connector) const;
   void m_OnPcBusConnectionStopConnect(void) const;
   void m_OnConnectionChanged(void);

   bool mq_EditMode;
   std::list<QDialog *> mc_Dialogs;
   C_GiSvPcBusConnector * mpc_SelectedBusConnectorItem;
   stw_opensyde_gui_logic::C_SebUnoTopBaseManager mc_UndoManager;
   C_SyvSeContextMenuManager mc_ContextMenuManager;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
