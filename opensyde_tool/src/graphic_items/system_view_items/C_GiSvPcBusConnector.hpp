//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       PC bus connection (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIPCBUSCONNECTOR_H
#define C_GIPCBUSCONNECTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiLiBusConnectorBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvPcBusConnector :
   public C_GiLiBusConnectorBase
{
public:
   C_GiSvPcBusConnector(const uint64_t ou64_UniqueId, const uint32_t ou32_ViewIndex,
                        const std::vector<QPointF> & orc_InteractionPoints, const C_GiLiBus * const opc_BusItem,
                        C_GiBiConnectableItem * const opc_GenericItem);
   int32_t type(void) const override;
   void GenerateHint(void) override;
   //lint -e{1735} Suppression, because default parameters are identical
   void SetAnimated(const bool oq_Active, const bool oq_Inverse = false, const bool oq_SpeedUp = false,
                    const QPolygonF oc_Polygon = QPolygonF(), const bool oq_ShowOrignalLine = true) override;
   void UpdateData(void) const;
   void SetEditMode(const bool oq_Active);

protected:
   void m_OnIterationGenericInteractionPointMove(QGraphicsItem * const opc_HighestParentItem,
                                                 const QPointF & orc_CurPos, bool & orq_RestoreMouseCursor) override;
   bool m_OnGenericInteractionPointMouseRelease(const QPointF & orc_ScenePos) override;
   void m_OnBusChange(const QPointF & orc_ScenePos) override;
   void m_AdaptStyleToBus(const C_GiLiBus * const opc_Bus) override;

private:
   const uint32_t mu32_ViewIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
