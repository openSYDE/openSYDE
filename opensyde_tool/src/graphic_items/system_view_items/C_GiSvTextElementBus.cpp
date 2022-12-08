//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text element for system view bus (implementation)

   Text element for system view bus

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_GiSvTextElementBus.hpp"

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

   \param[in]       ors32_Index          Index of data element in system definition
   \param[in]       oru64_Id             Unique ID
   \param[in,out]   opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvTextElementBus::C_GiSvTextElementBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                           QGraphicsItem * const opc_Parent) :
   C_GiTextElementBus(ors32_Index, oru64_Id, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle tooltip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvTextElementBus::GenerateHint(void)
{
   if ((this->mq_ErrorIconHovered == true) && (this->ms32_Index >= 0))
   {
      this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Invalid Bus Content"));
      this->SetDefaultToolTipContent(C_GtGetText::h_GetText("For further details switch to system definition"));
      this->SetDefaultToolTipType(C_NagToolTip::eERROR);
   }
   else
   {
      this->SetDefaultToolTipHeading("");
      this->SetDefaultToolTipContent("");
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
   }
}
