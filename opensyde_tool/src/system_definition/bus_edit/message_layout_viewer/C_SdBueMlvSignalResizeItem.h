//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for resizing items of signal items in the message layout viewer (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEMLVSIGNALRESIZEITEM_H
#define C_SDBUEMLVSIGNALRESIZEITEM_H

/* -- Includes ------------------------------------------------------------- */

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueMlvSignalResizeItem
{
public:
   C_SdBueMlvSignalResizeItem();
   virtual ~C_SdBueMlvSignalResizeItem();

private:
   //Avoid call
   C_SdBueMlvSignalResizeItem(const C_SdBueMlvSignalResizeItem &);
   C_SdBueMlvSignalResizeItem & operator =(const C_SdBueMlvSignalResizeItem &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
