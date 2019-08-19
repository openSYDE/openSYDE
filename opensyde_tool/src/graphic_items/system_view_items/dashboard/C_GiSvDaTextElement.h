//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDATEXTELEMENT_H
#define C_GISVDATEXTELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiTextElement.h"
#include "C_PuiSvDbDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaTextElement :
   public C_GiBiTextElement,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaTextElement(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                       const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                       const bool oq_Editable, QGraphicsItem * const opc_Parent = NULL);
   C_GiSvDaTextElement(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                       const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                       QGraphicsItem * const opc_Parent = NULL);
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   //GI base
   virtual void SetZValueCustom(const stw_types::float64 of64_ZValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
