//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view setup (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSESCENE_HPP
#define C_SYVSESCENE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>
#include "stwtypes.hpp"

#include "C_SyvTopologyBaseScene.hpp"
#include "C_SebUnoTopBaseManager.hpp"
#include "C_SyvSeContextMenuManager.hpp"

#include "C_GiNode.hpp"
#include "C_GiSvPcBusConnector.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeScene :
   public C_SyvTopologyBaseScene
{
   Q_OBJECT

public:
   C_SyvSeScene(const uint32_t ou32_ViewIndex = 0, QObject * const opc_Parent = NULL);
   ~C_SyvSeScene(void) override;

   void PrepareToClose(void);

   void SetEditMode(const bool oq_Active);
   void Save(void) const;
   void ReloadViewData(void);
   void ClearUndoStack(void);

   void SetAllNodesConnected(const bool oq_Connected);

   bool IsAnyItemAddable(void) const override;
   bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;
   bool IsSceneRubberBandAvailable(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorCheck(void) const;
   void SigConnectionChange(void) const;

protected:
   C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   stw::opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   C_GiNode * m_CreateNode(const int32_t & ors32_Index, const uint64_t & oru64_Id, const float64_t & orf64_Width,
                           const float64_t & orf64_Height, QGraphicsItem * const opc_Parent) override;

   void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;
   void m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem) override;
   void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvSeScene(const C_SyvSeScene &);
   C_SyvSeScene & operator =(const C_SyvSeScene &) &; //lint !e1511 //we want to hide the base func.

   void m_Edit(QGraphicsItem * const opc_GraphicsItem) const;
   void m_SelectionChanged(void);
   void m_OnPcBusConnectionStartConnect(const C_GiSvPcBusConnector::E_ConnectState & ore_ConnectState,
                                        const QGraphicsItem * const opc_Item,
                                        const stw::opensyde_core::C_OscSystemBus::E_Type * const ope_Type,
                                        const C_GiLiBusConnectorBase * const opc_Connector) const;
   void m_OnPcBusConnectionStopConnect(void) const;
   void m_OnConnectionChanged(void);

   bool mq_EditMode;
   std::list<QDialog *> mc_Dialogs;
   C_GiSvPcBusConnector * mpc_SelectedBusConnectorItem;
   stw::opensyde_gui_logic::C_SebUnoTopBaseManager mc_UndoManager;
   C_SyvSeContextMenuManager mc_ContextMenuManager;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
