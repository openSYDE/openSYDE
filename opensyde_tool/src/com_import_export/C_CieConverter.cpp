//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data conversion structure

   The structure matches the information provided by a DBC file.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#include "precomp_headers.hpp"

#include <QApplication>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CieConverter.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks messages on the basis of CAN Id
 *
   Compare the CAN Id for comparing message

   \param[in]       orc_Cmp    Message to be compared

   \return
   True if Current smaller than orc_Cmp
   False otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CieConverter::C_CieNodeMessage::operator <(const C_CieConverter::C_CieNodeMessage & orc_Cmp) const
{
   bool q_Return = false;

   if (this->c_CanMessage.u32_CanId < orc_Cmp.c_CanMessage.u32_CanId)
   {
      q_Return = true;
   }
   return q_Return;
};

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks messages on the basis of CAN Id

   Compare the CAN Id for comparing message

   \param[in]       orc_Cmp    Message to be compared

   \return
   True if Current is equal to orc_Cmp
   False otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CieConverter::C_CieNodeMessage::operator ==(const C_CieConverter::C_CieNodeMessage & orc_Cmp) const
{
   bool q_Return = false;

   if (this->c_CanMessage.u32_CanId == orc_Cmp.c_CanMessage.u32_CanId)
   {
      q_Return = true;
   }
   return q_Return;
};
