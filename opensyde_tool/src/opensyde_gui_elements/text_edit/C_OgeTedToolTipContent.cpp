//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for displaying content of tooltips (implementation)

   This text edit is similar to a label but has better word wrapping options.
   The size is dynamically calculated in dependence of text length and resulting
   number of lines.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QAbstractTextDocumentLayout>

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_OgeTedToolTipContent.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTedToolTipContent::C_OgeTedToolTipContent(QWidget * const opc_Parent) :
   QTextEdit(opc_Parent)
{
   this->setReadOnly(true);
   this->setFrameStyle(static_cast<int32_t>(QFrame::NoFrame));
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
   this->document()->setDocumentMargin(0.0); // no margin around the text wanted

   connect(this, &C_OgeTedToolTipContent::textChanged, this, &C_OgeTedToolTipContent::m_AdjustMinimumSize);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust minimum size of text edit to text length.

   First get convenient width (either with of longest line or a fixed maximum width)
   and calculate resulting height.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedToolTipContent::m_AdjustMinimumSize()
{
   const QString c_Text = this->toPlainText();
   QStringList c_Strings = c_Text.split('\n');
   QString c_LongestText;
   int32_t s32_LongestTextLength = 0;
   int32_t s32_Width = 630;
   int32_t s32_Height;

   const int32_t s32_MAX_WIDTH = 630;
   const int32_t s32_MAX_HEIGHT = 700;

   QFont c_Font = mc_STYLE_GUIDE_FONT_SEMIBOLD_13;

   c_Font.setPixelSize(c_Font.pointSize());
   const QFontMetrics c_FontMetric(c_Font);

   // get longest string in the string list (i.e. longest line when printed without automatic text warp)
   for (int32_t s32_Pos = 0; s32_Pos < c_Strings.length(); s32_Pos++)
   {
      if (s32_LongestTextLength < c_Strings[s32_Pos].length())
      {
         c_LongestText = c_Strings[s32_Pos];
         s32_LongestTextLength = c_LongestText.length();
      }
   }

   // set width to needed space if the line is not longer than 100 characters
   if (s32_LongestTextLength < 100)
   {
      s32_Width = static_cast<int32_t>(c_FontMetric.boundingRect(c_LongestText).width()) +
                  static_cast<int32_t>(2 * this->frameWidth()) + 6; // extra 6 for bigger letters in a second line with
                                                                    // same character number
   }

   // make sure the text edit gets not wider than maximum
   s32_Width = std::min(s32_Width, s32_MAX_WIDTH);

   s32_Height =
      static_cast<int32_t>(c_FontMetric.boundingRect(0, 0, s32_Width, 0,
                                                     static_cast<uint32_t>(Qt::TextWrapAnywhere), c_Text).height()) +
      static_cast<int32_t>(2 * this->frameWidth()) + 2;

   // make sure the text edit gets not higher than maximum
   s32_Height = std::min(s32_Height, s32_MAX_HEIGHT);

   this->setMinimumWidth(s32_Width);
   this->setMaximumWidth(s32_Width);
   this->setMinimumHeight(s32_Height);
   this->setMaximumHeight(s32_Height);
}
