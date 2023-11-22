//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view node user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEWNODE_HPP
#define C_USSYSTEMVIEWNODE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>
#include <QMap>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemViewNode
{
public:
   C_UsSystemViewNode(void);

   const QVector<bool> & GetSectionsExpanded(void) const;
   void SetSectionsExpanded(const QVector<bool> & orc_SectionsExpanded);

   const QMap<uint32_t, float64_t> & GetUpdateDataRateHistory() const;
   void AddUpdateDataRate(const uint32_t ou32_Checksum, const float64_t of64_Value);

private:
   QVector<bool> mc_SectionsExpanded;
   QMap<uint32_t, float64_t > mc_UpdateDataRateHistory;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
