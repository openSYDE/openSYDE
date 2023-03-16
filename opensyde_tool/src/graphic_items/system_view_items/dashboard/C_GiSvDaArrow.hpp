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
#include "C_GiBiArrow.hpp"
#include "C_PuiSvDbDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaArrow :
   public C_GiBiArrow,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaArrow(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                 const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                 const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
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
