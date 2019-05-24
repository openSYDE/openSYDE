//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for graphics items with system view dashboard data elements (implementation)

   Base class for graphics items with system view dashboard data elements

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_PuiSvDbDataElement.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] oru32_ViewIndex      Index of system view
   \param[in] oru32_DashboardIndex Index of dashboard in system view
   \param[in] ors32_DataIndex      Index of connected data item
   \param[in] ore_Type             Type of data
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElement::C_PuiSvDbDataElement(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                           const sint32 & ors32_DataIndex, const E_Type & ore_Type) :
   C_PuiBsDataElement(ors32_DataIndex),
   mu32_ViewIndex(oru32_ViewIndex),
   mu32_DashboardIndex(oru32_DashboardIndex),
   me_Type(ore_Type)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElement::~C_PuiSvDbDataElement(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt index if array was resized

   \param[in]   ore_Type      Type of affected items
   \param[in]   ors32_Index   Index of which segment
   \param[in]   ore_Action    Type of change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElement::SyncIndex(const E_Type & ore_Type, const stw_types::sint32 & ors32_Index,
                                     const E_Action & ore_Action)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard index

   \param[in] orc_Value New dashboard index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElement::SetDashboardIndex(const uint32 ou32_Value)
{
   this->mu32_DashboardIndex = ou32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget type

   \return
   Current widget type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElement::E_Type C_PuiSvDbDataElement::GetWidgetType(void) const
{
   return this->me_Type;
}
