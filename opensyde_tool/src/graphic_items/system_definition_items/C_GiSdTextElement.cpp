//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system definition (implementation)

   Custom text element graphics item in system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSdTextElement.hpp"
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
   \param[in]     oq_Editable Flag for editing the content of the text element
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdTextElement::C_GiSdTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id, const bool oq_Editable,
                                     QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_Id, oq_Editable, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eTEXT_ELEMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index Index of data element in system definition
   \param[in]     oru64_Id    Unique ID
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdTextElement::C_GiSdTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                     QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_Id, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eTEXT_ELEMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdTextElement::LoadData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      const C_PuiBsTextElement & rc_UiTextElementData =
         C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements[static_cast<uint32_t>(this->ms32_Index)];

      this->m_LoadTextElementData(&rc_UiTextElementData, false);
      //Object name for test
      this->setObjectName(static_cast<QString>("Meta: %1").arg(rc_UiTextElementData.c_UiText));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdTextElement::UpdateData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      C_PuiBsTextElement * const pc_Item =
         &C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements[this->ms32_Index];

      this->m_UpdateTextElementData(pc_Item, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdTextElement::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.erase(
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_TextElements.begin() +
      ms32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdTextElement::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiTextElement::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}
