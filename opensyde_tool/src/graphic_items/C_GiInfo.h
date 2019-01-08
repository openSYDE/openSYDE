//-----------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIINFO_H
#define C_GIINFO_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include <QSvgRenderer>
#include <QGraphicsRectItem>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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
   void SetSvg(const QString & orc_Value);
   void SetBackgroundColor(const QColor & orc_Value);
   void SetTextColor(const QColor & orc_Value, const stw_types::sint32 os32_Alignment);
   void SetIconSize(const stw_types::sint32 os32_IconSize);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget) override;
   //lint -restore

private:
   QString mc_Text;
   QColor mc_BackgroundColor;
   QColor mc_TextColor;
   stw_types::sint32 ms32_IconSize;
   QFont mc_TextFont;
   stw_types::sint32 ms32_TextAlignment;
   QSvgRenderer * mpc_SvgRenderer;

   void m_Update(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
