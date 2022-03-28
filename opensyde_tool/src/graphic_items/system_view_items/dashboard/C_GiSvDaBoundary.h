//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary in system view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDABOUNDARY_H
#define C_GISVDABOUNDARY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiBoundary.h"
#include "C_PuiSvDbDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaBoundary :
   public C_GiBiBoundary,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvDaBoundary(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                    const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                    const stw_types::float64 of64_Width = mhf64_MIN_WIDTH_BOUNDARY,
                    const stw_types::float64 of64_Height = mhf64_MIN_HEIGHT_BOUNDARY,
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
