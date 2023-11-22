//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics scene for system view update (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPSCENE_HPP
#define C_SYVUPSCENE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"

#include "C_GiNode.hpp"
#include "C_PuiSvData.hpp"
#include "C_GiSvNodeData.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_SyvTopologyBaseScene.hpp"
#include "C_SyvRoRouteCalculation.hpp"
#include "C_OscSuSequencesNodeStates.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpScene :
   public C_SyvTopologyBaseScene
{
   Q_OBJECT

public:
   C_SyvUpScene(const uint32_t ou32_ViewIndex = 0, QObject * const opc_Parent = NULL);
   ~C_SyvUpScene() override;

   void SetConnecting(const bool oq_Active) const;
   void SetConnected(const bool oq_Active) const;
   void SetUpdating(const bool oq_Active) const;
   void UpdateDeviceInformation(const std::vector<uint32_t> & orc_NodeIndexes,
                                const std::vector<stw::opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void StartConnectionAnimation(void) const;
   void StartProgressAnimation(const uint32_t ou32_NodeIndex) const;
   void StopProgressAnimation(const bool oq_Abort, const uint32_t ou32_FailedApplicationIndex,
                              const bool oq_StopUpdateingState) const;
   void SetNodeError(const uint32_t ou32_NodeIndex) const;
   std::vector<uint32_t> GetActiveNoneThirdPartyNodeIndices(void) const;
   void CheckUpdateDisabledState(void) const;

   void SetNodeConnectStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeConnectStates> & orc_NodeStates,
                             const C_GiSvNodeData::C_GiSvNodeDataPreconditionErrors & orc_NodePreconditionErrors);
   void SetNodeUpdateStates(const std::vector<stw::opensyde_core::C_OscSuSequencesNodeUpdateStates> & orc_NodeStates);

   bool IsAnyItemAddable(void) const override;
   bool IsItemMovable(const QGraphicsItem * const opc_Item) const override;
   bool IsItemSelectable(const QGraphicsItem * const opc_Item) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDiscardInfo(const uint32_t ou32_NodeIndex);

protected:
   C_SebBaseContextMenuManager * m_GetContextMenuManager(void) override;
   stw::opensyde_gui_logic::C_SebUnoBaseManager * m_GetUndoManager(void) override;

   C_GiNode * m_CreateNode(const int32_t & ors32_Index, const uint64_t & oru64_Id, const float64_t & orf64_Width,
                           const float64_t & orf64_Height, QGraphicsItem * const opc_Parent) override;

   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem) override;

private:
   static const float64_t mhf64_BUS_ANIMATION_TOLERANCE;

   //Avoid call
   C_SyvUpScene(const C_SyvUpScene &);
   C_SyvUpScene & operator =(const C_SyvUpScene &) &; //lint !e1511 //we want to hide the base func.

   void m_HandleBusConnectorInProgressAnimationStart(C_GiLiBusConnector & orc_BusConnector,
                                                     const stw::opensyde_gui_logic::C_SyvRoRouteCalculation & orc_RoutingCalculation, const stw::opensyde_core::C_OscRoutingRoute & orc_Route, const stw::opensyde_gui_logic::C_PuiSvData & orc_View, const uint32_t ou32_NodeIndex)
   const;
   bool m_IsLastBusConnectorInProgressAnimationToNode(
      const stw::opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_BusConnectionData,
      const C_GiLiBus & orc_BusItem,
      const stw::opensyde_gui_logic::C_SyvRoRouteCalculation & orc_RoutingCalculation,
      const stw::opensyde_core::C_OscRoutingRoute & orc_Route,
      const stw::opensyde_gui_logic::C_PuiSvData & orc_View, const uint32_t ou32_NodeIndex)
   const;
   int32_t m_StartProgressAnimationBusses(const stw::opensyde_gui_logic::C_SyvRoRouteCalculation & orc_Calc,
                                          const uint32_t ou32_NodeIndex) const;
   static int32_t mh_GetAnimationPath(const QPointF & orc_PointStart, const QPointF & orc_PointEnd,
                                      const std::vector<QPointF> & orc_UiInteractionPoints, QPolygonF & orc_Path,
                                      bool & orq_Inverse);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
