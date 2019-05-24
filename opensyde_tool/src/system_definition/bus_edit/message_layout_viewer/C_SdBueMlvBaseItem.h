//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for the message layout viewer graphic items (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVBASEITEM_H
#define C_SDBUEMLVBASEITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>
#include <QColor>
#include <QString>
#include <QSizeF>
#include <QFont>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvBaseItem :
   public QGraphicsItem
{
public:
   C_SdBueMlvBaseItem(const QColor & orc_BackgroundColor, const QColor & orc_FontColor, const QFont & orc_Font,
                      const QString & orc_Text, const bool oq_DrawText, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_SdBueMlvBaseItem();

   void SetText(const QString & orc_Text);
   void SetSize(const QSizeF & orc_Size);
   void SetFont(const QFont & orc_Font);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget);
   virtual QRectF boundingRect() const;
   //lint -restore

protected:
   const QColor mc_BackgroundColorDefault;
   QColor mc_DrawBackgroundColor;
   const QColor mc_FontColorDefault;
   QColor mc_DrawFontColor;
   QFont mc_Font;
   QString mc_Text;
   QSizeF mc_Size;
   bool mq_DrawBackground;

private:
   //Avoid call
   C_SdBueMlvBaseItem(const C_SdBueMlvBaseItem &);
   C_SdBueMlvBaseItem & operator =(const C_SdBueMlvBaseItem &);

   const bool mq_DrawText;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
