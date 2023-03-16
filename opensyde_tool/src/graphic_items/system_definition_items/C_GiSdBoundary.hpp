//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary in system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISDBOUNDARY_H
#define C_GISDBOUNDARY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiBoundary.hpp"
#include "C_PuiSdDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSdBoundary :
   public C_GiBiBoundary,
   public stw::opensyde_gui_logic::C_PuiSdDataElement
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSdBoundary(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                  const float64_t of64_Width = mhf64_MIN_WIDTH_BOUNDARY,
                  const float64_t of64_Height = mhf64_MIN_HEIGHT_BOUNDARY, QGraphicsItem * const opc_Parent = NULL);

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
