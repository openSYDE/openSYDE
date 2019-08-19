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
#include "stwtypes.h"
#include <QGraphicsScene>

#include "C_PuiSvDbDataElement.h"
#include "C_SebUnoBaseManager.h"
#include "C_SyvDaDashboardSnapshot.h"
#include "C_PuiSvReadDataConfiguration.h"

#include "C_SebUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoManager :
   public C_SebUnoBaseManager
{
public:
   C_SyvDaUnoManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent = NULL);
   virtual ~C_SyvDaUnoManager(void);

   virtual void DoDelete(const QList<QGraphicsItem *> & orc_Items) override;
   void DoAddGeneric(const C_PuiSvDbDataElement::E_Type & ore_Type, const stw_types::uint64 & oru64_UniqueID,
                     const QPointF & orc_NewPos, const stw_types::float64 of64_ZValue, const bool & orq_DarkModeDefault,
                     const QString & orc_AdditionalInformation = "");
   void DoAddSnapshot(const std::vector<stw_types::uint64> & oru64_UniqueIDs, const C_SyvDaDashboardSnapshot & orc_Snapshot,
                      const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                 stw_opensyde_gui_logic::C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                      const QPointF & orc_NewPos, const stw_types::float64 of64_HighestUsedZValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
