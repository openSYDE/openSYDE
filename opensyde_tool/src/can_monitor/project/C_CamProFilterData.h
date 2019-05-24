//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filter package and filter item data structure (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMPROFILTERDATA_H
#define C_CAMPROFILTERDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QList>

#include "stwtypes.h"
#include "C_OSCComMessageLogger.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamProFilterItemData :
   public stw_opensyde_core::C_OSCComMessageLoggerFilter
{
public:
   enum E_Type
   {
      eID_RANGE = 0,
      eID_SINGLE
   };

   C_CamProFilterItemData(void);
   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_Enabled;
   E_Type e_Type;
   QString c_DatabaseMatch;
};

class C_CamProFilterData
{
public:
   C_CamProFilterData(void);
   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QString c_Name;
   QString c_Comment;
   bool q_Enabled;
   QList<stw_opensyde_gui_logic::C_CamProFilterItemData> c_FilterItems;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
