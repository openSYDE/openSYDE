//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       A sizeable variant of QGraphicsPixmapItem (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPixmap>
#include <QImageReader>
#include <QImageWriter>

#include "stwtypes.hpp"
#include "C_GiImage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      orc_ImagePath    File path to image
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImage::C_GiImage(const QString & orc_ImagePath, QGraphicsItem * const opc_Parent) :
   QGraphicsPixmapItem(opc_Parent),
   C_GiBiSizeableItem(0.0, 0.0)
{
   // We also want to know the format for later saving. So we first load it by QImageReader to determine the format and
   // then read it into a QPixmap directly. The complete image data is then read from the file only once (QPixmap load).
   const QImageReader c_ImageReader(orc_ImagePath);

   this->mc_OriginalImageFormat = c_ImageReader.format(); // Empty if it cannot read the format, that is okay
   this->mc_OriginalPixmap.load(orc_ImagePath);

   // we want the image format for later saving, so if writing is not supported, default to PNG
   if (QImageWriter::supportedImageFormats().contains(this->mc_OriginalImageFormat.toLower()) == false)
   {
      this->mc_OriginalImageFormat = "png";
   }

   this->f64_Width = static_cast<float64_t>(this->mc_OriginalPixmap.width());
   this->f64_Height = static_cast<float64_t>(this->mc_OriginalPixmap.height());

   this->setPixmap(this->mc_OriginalPixmap);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      orc_Pixmap    Pixmap with image
   \param[in]      orc_Format    Image format
   \param[in]      of64_Width    Width
   \param[in]      of64_Height   Height
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImage::C_GiImage(const QPixmap & orc_Pixmap, const QByteArray & orc_Format, const float64_t of64_Width,
                     const float64_t of64_Height, QGraphicsItem * const opc_Parent) :
   QGraphicsPixmapItem(opc_Parent),
   C_GiBiSizeableItem(of64_Width, of64_Height),
   mc_OriginalPixmap(orc_Pixmap),
   mc_OriginalImageFormat(orc_Format)
{
   this->C_GiImage::Redraw();
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
   const QPixmap c_Pixmap = this->mc_OriginalPixmap.scaled(QSize(static_cast<int32_t>(this->f64_Width),
                                                                 static_cast<int32_t>(this->f64_Height)),
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the image type read by QImageReader (PNG, JPG, ...)

   \return     image type
*/
//----------------------------------------------------------------------------------------------------------------------
QByteArray C_GiImage::GetImageFormat(void) const
{
   return this->mc_OriginalImageFormat;
}
