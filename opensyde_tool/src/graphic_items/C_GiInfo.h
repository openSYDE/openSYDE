//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIINFO_H
#define C_GIINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <QMovie>
#include <QSvgRenderer>
#include <QGraphicsRectItem>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiInfo :
   public QObject,
   public QGraphicsRectItem
{
   Q_OBJECT

public:
   C_GiInfo(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiInfo(void);

   void SetTextFont(const QFont & orc_Value);
   void SetText(const QString & orc_Value);
   void SetGif(const QString & orc_Value);
   void SetSvg(const QString & orc_Value);
   void SetBackgroundColor(const QColor & orc_Value);
   void SetTextColor(const QColor & orc_Value, const stw_types::sint32 os32_Alignment);
   void SetIconSize(const stw_types::sint32 os32_IconSize);
   void UpdateTransform(const QTransform & orc_Transform);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget) override;

private:
   QString mc_Text;
   QColor mc_BackgroundColor;
   QColor mc_TextColor;
   stw_types::sint32 ms32_IconSize;
   QFont mc_TextFont;
   stw_types::sint32 ms32_TextAlignment;
   QSvgRenderer * mpc_SvgRenderer;
   QMovie * mpc_Movie;
   QTransform mc_Transform;

   void m_Update(void);
   void m_RestartGif(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
