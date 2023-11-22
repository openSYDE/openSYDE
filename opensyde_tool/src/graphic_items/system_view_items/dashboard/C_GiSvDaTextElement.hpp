//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATEXTELEMENT_HPP
#define C_GISVDATEXTELEMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiTextElement.hpp"
#include "C_PuiSvDbDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaTextElement :
   public C_GiBiTextElement,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaTextElement(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id, const bool oq_Editable,
                       QGraphicsItem * const opc_Parent = NULL);
   C_GiSvDaTextElement(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                       QGraphicsItem * const opc_Parent = NULL);
   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   void EnableEditContent(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
