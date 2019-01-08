//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for topology scene undo command stack (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNOTOPBASEMANAGER_H
#define C_SEBUNOTOPBASEMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SebUnoBaseManager.h"
#include "C_GiLiLineGroup.h"
#include "C_GiLiBusConnectorBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoTopBaseManager :
   public C_SebUnoBaseManager
{
public:
   C_SebUnoTopBaseManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent = NULL);
   virtual ~C_SebUnoTopBaseManager(void);

   virtual void RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items,
                                   const stw_types::sint32 & ors32_InteractionPointID,
                                   const QPointF & orc_PositionDifference);
   virtual void RegisterCompleteMoveStep(const QList<QGraphicsItem *> & orc_Items,
                                         const QPointF & orc_PositionDifference) override;
   virtual void RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_PositionDifference,
                                    const stw_types::sint32 & ors32_SubLineID) override;
   void SaveBusConnectorInitialPoints(const stw_opensyde_gui::C_GiLiBusConnectorBase * const opc_BusConnector);
   virtual void DoDelete(const QList<QGraphicsItem *> & orc_Items);

protected:
   virtual void m_CleanupAction(void);

private:
   std::vector<QPointF> mc_BusConnectorInitialPoints;

   bool m_CheckBusConnectorChange(const QList<QGraphicsItem *> & orc_Items);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
