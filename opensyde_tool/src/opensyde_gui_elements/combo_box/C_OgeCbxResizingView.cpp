//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base combo box class with resizing view functionality (implementation)

   Base combo box class with resizing view functionality

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QAbstractItemView>

#include "stwtypes.h"
#include "constants.h"
#include "C_OgeCbxResizingView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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
C_OgeCbxResizingView::C_OgeCbxResizingView(QWidget * const opc_Parent) :
   QComboBox(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resize view to longest entry & number of items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxResizingView::ResizeViewToContents(void) const
{
   sint32 s32_MaxWidth = 0;
   QFont c_Font = stw_opensyde_gui::mc_STYLE_GUIDE_FONT_REGULAR_12;

   c_Font.setPixelSize(c_Font.pointSize());
   {
      const QFontMetrics c_Metrics(c_Font);

      //Consider each item text
      for (sint32 s32_ItItem = 0; s32_ItItem < this->count(); ++s32_ItItem)
      {
         s32_MaxWidth =
            std::max(s32_MaxWidth, static_cast<sint32>(c_Metrics.horizontalAdvance(this->itemText(s32_ItItem))));
      }
      //Use combo box size as minimum, otherwise use longest entry
      //+ magic number (some offset to avoid cutting of any text)
      s32_MaxWidth = std::max(s32_MaxWidth + 10L, static_cast<sint32>(this->minimumSizeHint().width()));
      //5: Padding left
      this->view()->setMinimumWidth(static_cast<sintn>(s32_MaxWidth + 5L));
   }
   //Height: Number of items + border
   //27: Item height
   //2: Top & bottom border
   this->view()->setMinimumHeight((this->count() * 27) + 2);
}
