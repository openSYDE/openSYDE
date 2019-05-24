//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user in system definition (implementation)

   Informational Line / Arrow for user in system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiSdArrow.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index Index of data element in system definition
   \param[in]     oru64_ID    Unique ID
   \param[in]     opc_Points  Points for line
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdArrow::C_GiSdArrow(const sint32 & ors32_Index, const uint64 & oru64_ID,
                         const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiBiArrow(oru64_ID, opc_Points, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eLINE_ARROW)
{
   if (opc_Points == NULL)
   {
      if ((this->ms32_Index >= 0) &&
          (static_cast<uint32>(this->ms32_Index) < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size()))
      {
         C_PuiBsLineArrow & rc_ArrowData = C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows[this->ms32_Index];
         Init(rc_ArrowData.c_UIInteractionPoints);
         this->LoadData();
      }
   }

   m_GenerateArrows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdArrow::LoadData(void)
{
   if ((this->ms32_Index >= 0) &&
       (static_cast<uint32>(this->ms32_Index) < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size()))
   {
      m_LoadFromData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows[this->ms32_Index]);
   }
   //Object name for test
   this->setObjectName(QString("Meta: Line/Arrow"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdArrow::UpdateData(void)
{
   if ((this->ms32_Index >= 0) &&
       (static_cast<uint32>(this->ms32_Index) < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size()))
   {
      m_UpdateData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows[this->ms32_Index]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdArrow::DeleteData(void)
{
   if ((this->ms32_Index >= 0) &&
       (static_cast<uint32>(this->ms32_Index) < C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.size()))
   {
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.erase(
         C_PuiSdHandler::h_GetInstance()->c_Elements.c_LineArrows.begin() + static_cast<uint32>(this->ms32_Index));
   }
}
