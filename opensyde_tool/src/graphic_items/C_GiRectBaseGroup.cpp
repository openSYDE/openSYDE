//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all system definition items which are rectangle based (implementation)

   Base class for all system definition items which are rectangle based

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiRectBaseGroup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index            Index of data element in system definition
   \param[in]     ore_Type               Type for data storage
   \param[in]     oru64_Id               Unique ID
   \param[in]     of64_MinWidth          Minimum width of node
   \param[in]     of64_MinHeight         Minimum height of node
   \param[in]     of64_ActionPointOffset Action point offset
   \param[in]     oq_KeepAspectRatio     Flag if the rectangle should always keep its initial aspect ratio
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRectBaseGroup::C_GiRectBaseGroup(const int32_t & ors32_Index, const E_Type & ore_Type, const uint64_t & oru64_Id,
                                     const float64_t of64_MinWidth, const float64_t of64_MinHeight,
                                     const float64_t of64_ActionPointOffset, const bool oq_KeepAspectRatio,
                                     QGraphicsItem * const opc_Parent) :
   C_GiBiRectBaseGroup(oru64_Id, of64_MinWidth, of64_MinHeight, of64_ActionPointOffset, oq_KeepAspectRatio, opc_Parent),
   C_PuiSdDataElement(ors32_Index, ore_Type)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiRectBaseGroup::~C_GiRectBaseGroup(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiRectBaseGroup::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiRectBaseGroup::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}
