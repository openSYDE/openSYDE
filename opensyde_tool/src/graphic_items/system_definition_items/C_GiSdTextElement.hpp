//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISDTEXTELEMENT_H
#define C_GISDTEXTELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiTextElement.hpp"
#include "C_PuiSdDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSdTextElement :
   public C_GiBiTextElement,
   public stw::opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiSdTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id, const bool oq_Editable,
                     QGraphicsItem * const opc_Parent = NULL);
   C_GiSdTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id, QGraphicsItem * const opc_Parent = NULL);
   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
