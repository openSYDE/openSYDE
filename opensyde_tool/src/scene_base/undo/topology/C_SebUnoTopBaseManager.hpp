//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology scene undo command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBASEMANAGER_H
#define C_SEBUNOTOPBASEMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebUnoBaseManager.hpp"
#include "C_GiLiLineGroup.hpp"
#include "C_GiLiBusConnectorBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoTopBaseManager :
   public C_SebUnoBaseManager
{
public:
   C_SebUnoTopBaseManager(QGraphicsScene * const opc_Scene = NULL, QObject * const opc_Parent = NULL);
   ~C_SebUnoTopBaseManager(void) override;

   void RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items, const int32_t & ors32_InteractionPointId,
                           const QPointF & orc_PositionDifference) override;
   void RegisterCompleteMoveStep(const QList<QGraphicsItem *> & orc_Items,
                                 const QPointF & orc_PositionDifference) override;
   void RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_PositionDifference,
                            const int32_t & ors32_SubLineId) override;
   void SaveBusConnectorInitialPoints(const stw::opensyde_gui::C_GiLiBusConnectorBase * const opc_BusConnector);
   void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;

protected:
   void m_CleanupAction(void) override;

private:
   std::vector<QPointF> mc_BusConnectorInitialPoints;

   bool m_CheckBusConnectorChange(const QList<QGraphicsItem *> & orc_Items);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
