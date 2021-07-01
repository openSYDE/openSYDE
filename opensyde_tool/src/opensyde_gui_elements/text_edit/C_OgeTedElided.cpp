//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit with automatic eliding (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QTextLayout>

#include "stwtypes.h"

#include "C_OgeTedElided.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTedElided::C_OgeTedElided(QWidget * const opc_Parent) :
   C_OgeTedContextMenuBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the elided text

   Implemented like:
   http://doc.qt.io/qt-5/qtwidgets-widgets-elidedlabel-example.html

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedElided::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this->viewport());
   const QFontMetrics c_FontMetrics = c_Painter.fontMetrics();
   QString c_Content = this->toPlainText();
   const sintn sn_LineSpacing = c_FontMetrics.lineSpacing();
   // Offset of the border
   sintn sn_PosY = 4;
   const sintn sn_LineWidth = this->width() - 8;

   Q_UNUSED(opc_Event)

   while (c_Content != "")
   {
      sintn sn_Counter = 0;
      QString c_Line = "";
      sintn sn_ActLineWidth = 0;
      const sintn sn_NextLinePosY = sn_PosY + sn_LineSpacing;

      // search end line char or the last fitting letter
      while (c_Content != "")
      {
         if (sn_Counter < c_Content.length())
         {
            // minimum one char is left, check the length
            sn_ActLineWidth += c_FontMetrics.horizontalAdvance(c_Content[sn_Counter]);

            if ((sn_ActLineWidth < sn_LineWidth) &&
                (c_Content.at(sn_Counter) != '\n'))
            {
               // this char fits in this line
               c_Line += c_Content.mid(sn_Counter, 1);
               // checking next char
               ++sn_Counter;
            }
            else
            {
               // line finished
               if (c_Content.at(sn_Counter) != '\n')
               {
                  // update the original string
                  c_Content = c_Content.mid(sn_Counter);
               }
               else
               {
                  // update the original string
                  if ((sn_Counter + 1) < c_Content.length())
                  {
                     c_Content = c_Content.mid(sn_Counter + 1);
                  }
                  else
                  {
                     c_Content = "";
                  }
               }
               break;
            }
         }
         else
         {
            // last char of the string
            // get the line string
            c_Line += c_Content.mid(sn_Counter, 1);
            // nothing left
            c_Content = "";
         }
      }

      if (this->height() >= (sn_NextLinePosY + sn_LineSpacing))
      {
         // it is not the last line, drawing of the original text
         c_Painter.drawText(QPoint(4, sn_PosY + c_FontMetrics.ascent()), c_Line);
         sn_PosY = sn_NextLinePosY;
      }
      else
      {
         // this is the last fitting line
         const QString c_ElidedLastLine = c_FontMetrics.elidedText(c_Line, Qt::ElideRight, this->width() - 16);

         c_Painter.drawText(QPoint(4, sn_PosY + c_FontMetrics.ascent()), c_ElidedLastLine);
         c_Content = "";
      }
   }
}
