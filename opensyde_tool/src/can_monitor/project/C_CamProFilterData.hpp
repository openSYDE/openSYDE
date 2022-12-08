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

#include "stwtypes.hpp"
#include "C_OscComMessageLogger.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CamProFilterItemData :
   public stw::opensyde_core::C_OscComMessageLoggerFilter
{
public:
   enum E_Type
   {
      eID_RANGE = 0,
      eID_SINGLE
   };

   C_CamProFilterItemData(void);
   void CalcHash(uint32_t & oru32_HashValue) const;

   bool q_Enabled;
   E_Type e_Type;
   QString c_DatabaseMatch;
};

class C_CamProFilterData
{
public:
   C_CamProFilterData(void);
   void CalcHash(uint32_t & oru32_HashValue) const;

   QString c_Name;
   QString c_Comment;
   bool q_Enabled;
   QList<stw::opensyde_gui_logic::C_CamProFilterItemData> c_FilterItems;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
