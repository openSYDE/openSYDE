//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for resizing items of signal items in the message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVSIGNALRESIZEITEM_HPP
#define C_SDBUEMLVSIGNALRESIZEITEM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvSignalResizeItem
{
public:
   C_SdBueMlvSignalResizeItem();
   virtual ~C_SdBueMlvSignalResizeItem();

private:
   //Avoid call
   C_SdBueMlvSignalResizeItem(const C_SdBueMlvSignalResizeItem &);
   C_SdBueMlvSignalResizeItem & operator =(const C_SdBueMlvSignalResizeItem &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
