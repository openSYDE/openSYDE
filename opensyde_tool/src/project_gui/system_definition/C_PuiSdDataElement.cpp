//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for graphics items with system definition data elements (implementation)

   Base class for graphics items with system definition data elements

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_PuiSdDataElement.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Standard constructor

   \param[in]  ors32_Index    Index of connected data item
   \param[in]  ore_Type       Type of data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdDataElement::C_PuiSdDataElement(const int32_t & ors32_Index, const E_Type & ore_Type) :
   C_PuiBsDataElement(ors32_Index),
   me_Type(ore_Type)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdDataElement::~C_PuiSdDataElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt index if array was resized

   \param[in]  ore_Type       Type of affected items
   \param[in]  ors32_Index    Index of which segment
   \param[in]  ore_Action     Type of change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdDataElement::SyncIndex(const E_Type & ore_Type, const int32_t & ors32_Index, const E_Action & ore_Action)
{
   if (ore_Type == me_Type)
   {
      if (ore_Action == E_Action::eDELETE)
      {
         if (ors32_Index < ms32_Index)
         {
            --ms32_Index;
         }
      }
      else
      {
         if (ors32_Index <= ms32_Index)
         {
            ++ms32_Index;
         }
      }
   }
}
