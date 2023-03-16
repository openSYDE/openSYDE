//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image without data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIIMAGEGROUPWITHOUTDATA_HPP
#define C_GIIMAGEGROUPWITHOUTDATA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QPixmap>
#include <QGraphicsSvgItem>

#include "stwtypes.hpp"

#include "C_GiBiRectBaseGroup.hpp"
#include "C_GiImage.hpp"
#include "C_GiSvgGraphicsItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiImageGroupWithoutData :
   public C_GiBiRectBaseGroup
{
public:
   C_GiImageGroupWithoutData(const uint64_t & oru64_Id, const QString & orc_ImagePath,
                             const bool oq_RequestLateImageInit, QGraphicsItem * const opc_Parent = NULL);

   C_GiImageGroupWithoutData(const uint64_t & oru64_Id, const float64_t of64_Width, const float64_t of64_Height,
                             const QPixmap & orc_Image, QGraphicsItem * const opc_Parent = NULL);

   ~C_GiImageGroupWithoutData(void) override;

   virtual void LoadData(void) = 0;
   virtual void UpdateData(void) = 0;
   virtual void DeleteData(void) = 0;

   QPixmap GetImage(void) const;

protected:
   void m_LateImageInit(const QString & orc_ImagePath);
   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;

   C_GiImage * mpc_Image;
   C_GiSvgGraphicsItem * mpc_SvgGraphicsItem;
   bool mq_IsSvgFlag;
   static const float64_t mhf64_MIN_WIDTH_IMAGE;
   static const float64_t mhf64_MIN_HEIGHT_IMAGE;

private:
   //Avoid call
   C_GiImageGroupWithoutData(const C_GiImageGroupWithoutData &);
   C_GiImageGroupWithoutData & operator =(const C_GiImageGroupWithoutData &) &; //lint !e1511 //we want to hide the base
                                                                                // func.

   void m_Init(void);
   void m_InitSvg(void);

   static const float64_t mhf64_ACTION_POINT_OFFSET_IMAGE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
