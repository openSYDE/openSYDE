//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for empty items of message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVEMPTYITEM_HPP
#define C_SDBUEMLVEMPTYITEM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include <QPixmap>

#include "C_SdBueMlvBaseItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvEmptyItem :
   public C_SdBueMlvBaseItem
{
public:
   C_SdBueMlvEmptyItem(const uint8_t ou8_Index = 0, QGraphicsItem * const opc_Parent = NULL);
   ~C_SdBueMlvEmptyItem() override;

   void SetIndex(const uint8_t ou8_Index);
   void SetError(const bool oq_Error);
   void SetActive(const bool oq_Active);
   void SetFontColor(const QColor & orc_Color);
   void RestoreFontColor(void);
   void SetDrawRectangle(const bool oq_DrawRectangle);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget) override;

private:
   //Avoid call
   C_SdBueMlvEmptyItem(const C_SdBueMlvEmptyItem &);
   C_SdBueMlvEmptyItem & operator =(const C_SdBueMlvEmptyItem &) &; //lint !e1511 //we want to hide the base function

   QColor mc_BackgroundColorError;
   QPixmap mc_ErrorIcon;
   bool mq_ErrorActive;

   bool mq_DrawRectangle;

   QColor mc_BackgroundColorInactive;
   QColor mc_FontColorInactive;
   bool mq_ItemActive;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
