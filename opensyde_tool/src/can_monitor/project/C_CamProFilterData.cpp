//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filter package and filter item data structure (implementation)

   A filter in openSYDE CAN monitor is more a filter package.
   Such a filter package consists of one or more openSYDE core
   filters and additional information such as a name and a comment.

   To avoid naming confusion consider:

      openSYDE CAN monitor       openSYDE core
      ---------------------------------------------------------
      filter item                filter
      filter                     package of one ore more filters

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamProFilterData.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor of filter class.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProFilterData::C_CamProFilterData(void)
{
   this->c_Name = "DefaultFilterName";
   this->c_Comment = "";
   this->q_Enabled = true;
   this->c_FilterItems.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProFilterData::CalcHash(uint32_t & oru32_HashValue) const
{
   // filter properties (name, comment, state)
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.toStdString().c_str(), this->c_Name.length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.toStdString().c_str(), this->c_Comment.length(),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enabled, sizeof(this->q_Enabled), oru32_HashValue);

   // filter items properties
   for (int32_t s32_Pos = 0; s32_Pos < this->c_FilterItems.size(); s32_Pos++)
   {
      this->c_FilterItems[s32_Pos].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor of filter item class.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProFilterItemData::C_CamProFilterItemData()
{
   this->q_Enabled = true;
   if (this->u32_StartId == this->u32_EndId)
   {
      e_Type = eID_SINGLE;
   }
   else
   {
      e_Type = eID_RANGE;
   }

   this->c_DatabaseMatch = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProFilterItemData::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_PassFilter, sizeof(this->q_PassFilter), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_ExtendedId, sizeof(this->u8_ExtendedId), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_EndId, sizeof(this->u32_EndId), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_StartId, sizeof(this->u32_StartId), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enabled, sizeof(this->q_Enabled), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
}
