//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user in system view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAARROW_H
#define C_GISVDAARROW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiArrow.h"
#include "C_PuiSvDbDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaArrow :
   public C_GiBiArrow,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaArrow(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                 const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                 const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
