//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image in system view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISVDAIMAGEGROUP_HPP
#define C_GISVDAIMAGEGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiImageGroup.hpp"
#include "C_PuiSvDbDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvDaImageGroup :
   public C_GiBiImageGroup,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaImageGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                      const int32_t & ors32_DataIndex, const uint64_t & oru64_Id, const QString & orc_ImagePath,
                      QGraphicsItem * const opc_Parent = NULL);
   C_GiSvDaImageGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                      const int32_t & ors32_DataIndex, const uint64_t & oru64_Id, const float64_t of64_Width,
                      const float64_t of64_Height, const QPixmap & orc_Image, const QByteArray & orc_Format,
                      QGraphicsItem * const opc_Parent = NULL);
   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
