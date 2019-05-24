//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIIMAGEGROUP_H
#define C_GIIMAGEGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "stwtypes.h"

#include "C_GiImageGroupWithoutData.h"
#include "C_PuiBsImage.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiImageGroup :
   public C_GiImageGroupWithoutData
{
public:
   C_GiBiImageGroup(const stw_types::uint64 & oru64_ID, const QString & orc_ImagePath,
                    QGraphicsItem * const opc_Parent = NULL);

   C_GiBiImageGroup(const stw_types::uint64 & oru64_ID, const stw_types::float64 of64_Width,
                    const stw_types::float64 of64_Height, const QPixmap & orc_Image,
                    QGraphicsItem * const opc_Parent = NULL);

   virtual stw_types::sintn type() const override;

protected:
   void m_UpdateData(stw_opensyde_gui_logic::C_PuiBsImage & orc_Data) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
