//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsPixmapItem (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPixmap>

#include "stwtypes.h"
#include "C_GiImage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_ImagePath  File path to image
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImage::C_GiImage(const QString & orc_ImagePath, QGraphicsItem * const opc_Parent) :
   QGraphicsPixmapItem(opc_Parent),
   C_GiBiSizeableItem(0.0, 0.0)
{
   this->mc_OriginalPixmap.load(orc_ImagePath);

   this->f64_Width = static_cast<float64>(this->mc_OriginalPixmap.width());
   this->f64_Height = static_cast<float64>(this->mc_OriginalPixmap.height());

   this->setPixmap(this->mc_OriginalPixmap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Pixmap   Pixmap with image
   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImage::C_GiImage(const QPixmap & orc_Pixmap, const float64 of64_Width, const float64 of64_Height,
                     QGraphicsItem * const opc_Parent) :
   QGraphicsPixmapItem(opc_Parent),
   C_GiBiSizeableItem(of64_Width, of64_Height),
   mc_OriginalPixmap(orc_Pixmap)
{
   this->Redraw();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImage::~C_GiImage()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing of the rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiImage::Redraw(void)
{
   // calculate the scaling on the original picture to get best quality
   QPixmap c_Pixmap = this->mc_OriginalPixmap.scaled(QSize(static_cast<sintn>(this->f64_Width),
                                                           static_cast<sintn>(this->f64_Height)),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation);

   this->setPixmap(c_Pixmap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the original image without compression

   \return     Original image without scaling
*/
//----------------------------------------------------------------------------------------------------------------------
QPixmap C_GiImage::GetImage(void) const
{
   return this->mc_OriginalPixmap;
}
