//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view node user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEWNODE_H
#define C_USSYSTEMVIEWNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>
#include <QMap>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemViewNode
{
public:
   C_UsSystemViewNode(void);

   const QVector<bool> & GetSectionsExpanded(void) const;
   void SetSectionsExpanded(const QVector<bool> & orc_SectionsExpanded);

   const QMap<stw_types::uint32, stw_types::float64> & GetUpdateDataRateHistory() const;
   void AddUpdateDataRate(const stw_types::uint32 ou32_Checksum, const stw_types::float64 of64_Value);

private:
   QVector<bool> mc_SectionsExpanded;
   QMap<stw_types::uint32, stw_types::float64 > mc_UpdateDataRateHistory;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
