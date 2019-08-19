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
#include "C_GiBiRectBaseGroup.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiRectBaseGroup :
   public C_GiBiRectBaseGroup,
   public stw_opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiRectBaseGroup(const stw_types::sint32 & ors32_Index, const C_PuiSdDataElement::E_Type & ore_Type,
                     const stw_types::uint64 & oru64_ID, const stw_types::float64 of64_MinWidth,
                     const stw_types::float64 of64_MinHeight, const stw_types::float64 of64_ActionPointOffset,
                     const bool oq_KeepAspectRatio, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiRectBaseGroup(void);

   //GI base
   virtual void SetZValueCustom(const stw_types::float64 of64_ZValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
