//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsPixmapItem (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIIMAGE_H
#define C_GIIMAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsPixmapItem>
#include <QString>

#include "C_GiBiSizeableItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiImage :
   public QGraphicsPixmapItem,
   public C_GiBiSizeableItem
{
public:
   C_GiImage(const QString & orc_ImagePath, QGraphicsItem * const opc_Parent = NULL);
   C_GiImage(const QPixmap & orc_Pixmap, const stw_types::float64 of64_Width, const stw_types::float64 of64_Height,
             QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiImage();

   virtual void Redraw(void) override;

   QPixmap GetImage(void) const;

private:
   QPixmap mc_OriginalPixmap; ///< Original image for saving
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
