//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view update (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPSCENE_H
#define C_SYVUPSCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"

#include "C_GiNode.h"
#include "C_PuiSvData.h"
#include "C_GiSvNodeData.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_SyvTopologyBaseScene.h"
#include "C_SyvRoRouteCalculation.h"
#include "C_OSCSuSequencesNodeStates.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpScene :
   public C_SyvTopologyBaseScene
{
   Q_OBJECT

public:
   C_SyvUpScene(const stw_types::uint32 ou32_ViewIndex = 0, QObject * const opc_Parent = NULL);
   virtual ~C_SyvUpScene();

   void SetConnecting(const bool oq_Active) const;
   void SetConnected(const bool oq_Active) const;
   void SetUpdating(const bool oq_Active) const;
   void UpdateDeviceInformation(const std::vector<stw_types::uint32> & orc_NodeIndexes,
                                const std::vector<stw_opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void StartConnectionAnimation(void) const;
   void StartProgressAnimation(const stw_types::uint32 ou32_NodeIndex) const;
   void StopProgressAnimation(const bool oq_Abort, const stw_types::uint32 ou32_FailedApplicationIndex,
                              const bool oq_StopUpdateingState) const;
   void SetNodeError(const stw_types::uint32 ou32_NodeIndex) const;
   std::vector<stw_types::uint32> GetActiveNoneThirdPartyNodeIndices(void) const;
   void CheckUpdateDisabledState(void) const;

   void SetNodeConnectStates(const std::vector<stw_opensyde_core::C_OSCSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SetNodeUpdateStates(const std::vector<stw_opensyde_core::C_OSCSuSequencesNodeUpdateStates> & orc_NodeStates);

   virtual bool IsAnyItemAddable(void) const override;
   virtual bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDiscardInfo(const stw_types::uint32 ou32_NodeIndex);

protected:
   virtual C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   virtual stw_opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   virtual C_GiNode * m_CreateNode(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                   const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_Height,
                                   QGraphicsItem * const opc_Parent) override;

   virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   virtual void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;

private:
   static const stw_types::float64 mhf64_BUS_ANIMATION_TOLERANCE;

   //Avoid call
   C_SyvUpScene(const C_SyvUpScene &);
   C_SyvUpScene & operator =(const C_SyvUpScene &); //lint !e1511 //we want to hide the base func.

   void m_HandleBusConnectorInProgressAnimationStart(C_GiLiBusConnector & orc_BusConnector,
                                                     const stw_opensyde_gui_logic::C_SyvRoRouteCalculation & orc_RoutingCalculation, const stw_opensyde_core::C_OSCRoutingRoute & orc_Route, const stw_opensyde_gui_logic::C_PuiSvData & orc_View, const stw_types::uint32 ou32_NodeIndex)
   const;
   bool m_IsLastBusConnectorInProgressAnimationToNode(
      const stw_opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_BusConnectionData, const C_GiLiBus & orc_BusItem,
      const stw_opensyde_gui_logic::C_SyvRoRouteCalculation & orc_RoutingCalculation,
      const stw_opensyde_core::C_OSCRoutingRoute & orc_Route, const stw_opensyde_gui_logic::C_PuiSvData & orc_View,
      const stw_types::uint32 ou32_NodeIndex)
   const;
   stw_types::sint32 m_StartProgressAnimationBusses(const stw_opensyde_gui_logic::C_SyvRoRouteCalculation & orc_Calc,
                                                    const stw_types::uint32 ou32_NodeIndex) const;
   static stw_types::sint32 mh_GetAnimationPath(const QPointF & orc_PointStart, const QPointF & orc_PointEnd,
                                                const std::vector<QPointF> & orc_UIInteractionPoints,
                                                QPolygonF & orc_Path, bool & orq_Inverse);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
