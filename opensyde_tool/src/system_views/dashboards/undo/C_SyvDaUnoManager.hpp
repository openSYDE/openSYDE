//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for dashboard undo command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOMANAGER_H
#define C_SYVDAUNOMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include <QGraphicsScene>

#include "C_PuiSvDbDataElement.hpp"
#include "C_SebUnoBaseManager.hpp"
#include "C_SyvDaDashboardSnapshot.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

#include "C_SebUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoManager :
   public C_SebUnoBaseManager
{
public:
   C_SyvDaUnoManager(QGraphicsScene * const opc_Scene = NULL, QObject * const opc_Parent = NULL);
   ~C_SyvDaUnoManager(void) override;

   void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;
   void DoAddGeneric(const C_PuiSvDbDataElement::E_Type & ore_Type, const uint64_t & oru64_UniqueId,
                     const QPointF & orc_NewPos, const float64_t of64_ZetValue, const bool & orq_DarkModeDefault,
                     const QString & orc_AdditionalInformation = "");
   void DoAddSnapshot(const std::vector<uint64_t> & orc_UniqueIds, const C_SyvDaDashboardSnapshot & orc_Snapshot,
                      const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                 stw::opensyde_gui_logic::C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                      const QPointF & orc_NewPos, const float64_t of64_HighestUsedZetValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
