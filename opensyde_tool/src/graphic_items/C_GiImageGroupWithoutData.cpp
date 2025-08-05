//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image without data (implementation)

   Implementation for drawing element image without data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>

#include "stwtypes.hpp"
#include "C_GiBiImageGroup.hpp"
#include "C_PuiSdDataElement.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiBsImage.hpp"
#include "gitypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64_t C_GiImageGroupWithoutData::mhf64_ACTION_POINT_OFFSET_IMAGE = 8.0;

const float64_t C_GiImageGroupWithoutData::mhf64_MIN_WIDTH_IMAGE = 10.0;
const float64_t C_GiImageGroupWithoutData::mhf64_MIN_HEIGHT_IMAGE = 10.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oru64_Id                  Unique ID
   \param[in]      orc_ImagePath             File path of image
   \param[in]      oq_RequestLateImageInit   Request late image init
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImageGroupWithoutData::C_GiImageGroupWithoutData(const uint64_t & oru64_Id, const QString & orc_ImagePath,
                                                     const bool oq_RequestLateImageInit,
                                                     QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_Id, mhf64_MIN_WIDTH_IMAGE,
                       mhf64_MIN_HEIGHT_IMAGE, mhf64_ACTION_POINT_OFFSET_IMAGE, true, opc_Parent),
   mpc_Image(NULL),
   mpc_SvgGraphicsItem(NULL),
   mq_IsSvgFlag(oq_RequestLateImageInit)
{
   if (oq_RequestLateImageInit == false)
   {
      m_LateImageInit(orc_ImagePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oru64_Id         Unique ID
   \param[in]      of64_Width       Width of image view
   \param[in]      of64_Height      Height of image view
   \param[in]      orc_Image        Image
   \param[in]      orc_Format       Image format
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiImageGroupWithoutData::C_GiImageGroupWithoutData(const uint64_t & oru64_Id, const float64_t of64_Width,
                                                     const float64_t of64_Height, const QPixmap & orc_Image,
                                                     const QByteArray & orc_Format, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_Id, mhf64_MIN_WIDTH_IMAGE,
                       mhf64_MIN_HEIGHT_IMAGE, mhf64_ACTION_POINT_OFFSET_IMAGE, true, opc_Parent),
   mpc_SvgGraphicsItem(NULL),
   mq_IsSvgFlag(false)
{
   // image
   this->mpc_Image = new C_GiImage(orc_Image, orc_Format, of64_Width, of64_Height);

   this->m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Image and the Qt memory management
C_GiImageGroupWithoutData::~C_GiImageGroupWithoutData()
{
   //Deleted via Qt parent mechanism
   this->mpc_Image = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the image

   \return  Actual image
*/
//----------------------------------------------------------------------------------------------------------------------
QPixmap C_GiImageGroupWithoutData::GetImage(void) const
{
   QPixmap c_Retval;

   if (this->mpc_Image != NULL)
   {
      c_Retval = this->mpc_Image->GetImage();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the image type read by QImageReader (PNG, JPG, ...)

   \return  Image type
*/
//----------------------------------------------------------------------------------------------------------------------
QByteArray C_GiImageGroupWithoutData::GetImageFormat(void) const
{
   QByteArray c_Retval;

   if (this->mpc_Image != NULL)
   {
      c_Retval = this->mpc_Image->GetImageFormat();
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Late image initialization

   \param[in]  orc_ImagePath  File path of image
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiImageGroupWithoutData::m_LateImageInit(const QString & orc_ImagePath)
{
   QFileInfo c_File;

   c_File.setFile(orc_ImagePath);
   if (c_File.exists() == true)
   {
      if ((c_File.suffix().compare("svg") == 0) && (this->mq_IsSvgFlag == true))
      {
         // svg
         this->mpc_SvgGraphicsItem = new C_GiSvgGraphicsItem(orc_ImagePath, 10.0, 10.0);
         this->mq_IsSvgFlag = true;
         this->m_InitSvg();
      }
      else
      {
         // image
         this->mpc_Image = new C_GiImage(orc_ImagePath);
         this->mq_IsSvgFlag = false;

         this->m_Init();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiImageGroupWithoutData::m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight)
{
   Q_UNUSED(of64_DiffWidth)
   Q_UNUSED(of64_DiffHeight)
   if (this->mq_IsSvgFlag == true)
   {
      if (this->mpc_SvgGraphicsItem != NULL)
      {
         this->mpc_SvgGraphicsItem->setRect(this->mpc_SvgGraphicsItem->rect().x(),
                                            this->mpc_SvgGraphicsItem->rect().y(),
                                            this->mpc_SvgGraphicsItem->rect().width() + of64_DiffWidth,
                                            this->mpc_SvgGraphicsItem->rect().height() + of64_DiffHeight);
         this->mpc_SvgGraphicsItem->update();
      }
   }
   else
   {
      if (this->mpc_Image != NULL)
      {
         this->mpc_Image->update();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiImageGroupWithoutData::m_Init(void)
{
   if (this->mpc_Image != NULL)
   {
      // Notify the base class about the image as biggest item as orientation. Very important!
      this->m_SetBiggestItem(*this->mpc_Image);

      this->addToGroup(this->mpc_Image);
   }

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_IMAGE);
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiImageGroupWithoutData::m_InitSvg(void)
{
   if (this->mpc_SvgGraphicsItem != NULL)
   {
      // Notify the base class about the image as biggest item as orientation. Very important!
      this->m_SetBiggestItem(*this->mpc_SvgGraphicsItem);
      this->addToGroup(this->mpc_SvgGraphicsItem);
   }
   // Init z order
   this->setZValue(mf64_ZORDER_INIT_IMAGE);
}
