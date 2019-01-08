//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for empty items of message layout viewer (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEMLVEMPTYITEM_H
#define C_SDBUEMLVEMPTYITEM_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include <QPixmap>

#include "C_SdBueMlvBaseItem.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueMlvEmptyItem :
   public C_SdBueMlvBaseItem
{
public:
   C_SdBueMlvEmptyItem(const stw_types::uint8 ou8_Index, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_SdBueMlvEmptyItem();

   void SetIndex(const stw_types::uint8 ou8_Index);
   void SetError(const bool oq_Error);
   void SetActive(const bool oq_Active);
   void SetFontColor(const QColor & orc_Color);
   void RestoreFontColor(void);
   void SetDrawRectangle(const bool oq_DrawRectangle);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget);
   //lint -restore

private:
   //Avoid call
   C_SdBueMlvEmptyItem(const C_SdBueMlvEmptyItem &);
   C_SdBueMlvEmptyItem & operator =(const C_SdBueMlvEmptyItem &);

   QColor mc_BackgroundColorError;
   QPixmap mc_ErrorIcon;
   bool mq_ErrorActive;

   bool mq_DrawRectangle;

   QColor mc_BackgroundColorInactive;
   QColor mc_FontColorInactive;
   bool mq_ItemActive;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
