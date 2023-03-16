//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all system definition items which are rectangle based (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIRECTBASEGROUP_H
#define C_GIRECTBASEGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiRectBaseGroup.hpp"
#include "C_PuiSdDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiRectBaseGroup :
   public C_GiBiRectBaseGroup,
   public stw::opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiRectBaseGroup(const int32_t & ors32_Index, const C_PuiSdDataElement::E_Type & ore_Type,
                     const uint64_t & oru64_Id, const float64_t of64_MinWidth, const float64_t of64_MinHeight,
                     const float64_t of64_ActionPointOffset, const bool oq_KeepAspectRatio,
                     QGraphicsItem * const opc_Parent = NULL);
   ~C_GiRectBaseGroup(void) override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
