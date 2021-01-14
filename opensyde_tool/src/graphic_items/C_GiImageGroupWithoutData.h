//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image without data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIIMAGEGROUPWITHOUTDATA_H
#define C_GIIMAGEGROUPWITHOUTDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QPixmap>

#include "stwtypes.h"

#include "C_GiBiRectBaseGroup.h"
#include "C_GiImage.h"
#include "C_GiSvgGraphicsItem.h"
#include <QGraphicsSvgItem>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiImageGroupWithoutData :
   public C_GiBiRectBaseGroup
{
public:
   C_GiImageGroupWithoutData(const stw_types::uint64 & oru64_ID, const QString & orc_ImagePath,
                             const bool oq_RequestLateImageInit, QGraphicsItem * const opc_Parent = NULL);

   C_GiImageGroupWithoutData(const stw_types::uint64 & oru64_ID, const stw_types::float64 of64_Width,
                             const stw_types::float64 of64_Height, const QPixmap & orc_Image,
                             QGraphicsItem * const opc_Parent = NULL);

   virtual ~C_GiImageGroupWithoutData(void);

   virtual void LoadData(void) = 0;
   virtual void UpdateData(void) = 0;
   virtual void DeleteData(void) = 0;

   QPixmap GetImage(void) const;

protected:
   void m_LateImageInit(const QString & orc_ImagePath);
   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth,
                                    const stw_types::float64 of64_DiffHeight) override;

   C_GiImage * mpc_Image;
   C_GiSvgGraphicsItem * mpc_SvgGraphicsItem;
   bool mq_IsSvgFlag;
   static const stw_types::float64 mhf64_MinWidthImage;
   static const stw_types::float64 mhf64_MinHeightImage;

private:
   //Avoid call
   C_GiImageGroupWithoutData(const C_GiImageGroupWithoutData &);
   C_GiImageGroupWithoutData & operator =(const C_GiImageGroupWithoutData &); //lint !e1511 //we want to hide the base
                                                                              // func.

   void m_Init(void);
   void m_InitSvg(void);

   static const stw_types::float64 mhf64_ActionPointOffsetImage;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
