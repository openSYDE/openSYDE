//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for border items of message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVBORDERITEM_H
#define C_SDBUEMLVBORDERITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "stwtypes.hpp"

#include "C_SdBueMlvBaseItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvBorderItem :
   public C_SdBueMlvBaseItem
{
public:
   C_SdBueMlvBorderItem(const QString & orc_Title = "", const uint8_t ou8_Index = 0U,
                        QGraphicsItem * const opc_Parent = NULL);
   ~C_SdBueMlvBorderItem() override;

   void SetIndex(const uint8_t ou8_Index);
   void SetActive(const bool oq_Active);

private:
   //Avoid call
   C_SdBueMlvBorderItem(const C_SdBueMlvBorderItem &);
   C_SdBueMlvBorderItem & operator =(const C_SdBueMlvBorderItem &) &; //lint !e1511 //we want to hide the base function

   const QString mc_Title;
   QColor mc_BackgroundColorInactive;
   QColor mc_FontColorInactive;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
