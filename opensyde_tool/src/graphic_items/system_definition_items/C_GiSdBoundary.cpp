//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary in system definition (implementation)

   Implementation for drawing element boundary in system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSdBoundary.hpp"
#include "C_PuiSdHandler.hpp"

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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index Index of data element in system definition
   \param[in]     oru64_Id    Unique ID
   \param[in]     of64_Width  Width of node
   \param[in]     of64_Height Height of node
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdBoundary::C_GiSdBoundary(const int32_t & ors32_Index, const uint64_t & oru64_Id, const float64_t of64_Width,
                               const float64_t of64_Height, QGraphicsItem * const opc_Parent) :
   C_GiBiBoundary(oru64_Id, of64_Width, of64_Height, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eBOUNDARY)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdBoundary::LoadData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      m_LoadFromData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[this->ms32_Index], false);
   }
   //Object name for test
   this->setObjectName("Meta: Boundary");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdBoundary::UpdateData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      m_UpdateData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries[this->ms32_Index], false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdBoundary::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.erase(
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_Boundaries.begin() + ms32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdBoundary::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiBoundary::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}
