//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit with automatic eliding (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QTextLayout>

#include "stwtypes.hpp"

#include "C_OgeTedElided.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

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
   const int32_t s32_LineSpacing = c_FontMetrics.lineSpacing();
   // Offset of the border
   int32_t s32_PosVertical = 4;
   const int32_t s32_LineWidth = this->width() - 8;

   Q_UNUSED(opc_Event)

   while (c_Content != "")
   {
      int32_t s32_Counter = 0;
      QString c_Line = "";
      int32_t s32_ActLineWidth = 0;
      const int32_t s32_NextLinePosVertical = s32_PosVertical + s32_LineSpacing;

      // search end line char or the last fitting letter
      while (c_Content != "")
      {
         if (s32_Counter < c_Content.length())
         {
            // minimum one char is left, check the length
            s32_ActLineWidth += c_FontMetrics.horizontalAdvance(c_Content[s32_Counter]);

            if ((s32_ActLineWidth < s32_LineWidth) &&
                (c_Content.at(s32_Counter) != '\n'))
            {
               // this char fits in this line
               c_Line += c_Content.mid(s32_Counter, 1);
               // checking next char
               ++s32_Counter;
            }
            else
            {
               // line finished
               if (c_Content.at(s32_Counter) != '\n')
               {
                  // update the original string
                  c_Content = c_Content.mid(s32_Counter);
               }
               else
               {
                  // update the original string
                  if ((s32_Counter + 1) < c_Content.length())
                  {
                     c_Content = c_Content.mid(s32_Counter + 1);
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
            c_Line += c_Content.mid(s32_Counter, 1);
            // nothing left
            c_Content = "";
         }
      }

      if (this->height() >= (s32_NextLinePosVertical + s32_LineSpacing))
      {
         // it is not the last line, drawing of the original text
         c_Painter.drawText(QPoint(4, s32_PosVertical + c_FontMetrics.ascent()), c_Line);
         s32_PosVertical = s32_NextLinePosVertical;
      }
      else
      {
         // this is the last fitting line
         const QString c_ElidedLastLine = c_FontMetrics.elidedText(c_Line, Qt::ElideRight, this->width() - 16);

         c_Painter.drawText(QPoint(4, s32_PosVertical + c_FontMetrics.ascent()), c_ElidedLastLine);
         c_Content = "";
      }
   }
}
