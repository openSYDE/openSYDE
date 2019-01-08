//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom combo box for table (implementation)

   Custom combo box for table.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeCbxIconDelegate.h"
#include <QAbstractItemView>
#include "stwtypes.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_OgeCbxTable.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeCbxTable::C_OgeCbxTable(QWidget * const opc_Parent) :
   QComboBox(opc_Parent)
{
   //this code allows to handle the QAbstractItemView::item in stylesheets
   C_OgeCbxIconDelegate * const pc_ItemDelegate = new C_OgeCbxIconDelegate();

   this->setItemDelegate(pc_ItemDelegate);
   //lint -e{429}  no memory leak because of the parent all elements and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     11.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeCbxTable::~C_OgeCbxTable(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Resize view to longest entry & number of items

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeCbxTable::ResizeViewToContents(void) const
{
   sint32 s32_MaxWidth = 0;
   QFont c_Font = stw_opensyde_gui::mc_STYLE_GUIDE_FONT_REGULAR_14;

   c_Font.setPixelSize(c_Font.pointSize());
   {
      const QFontMetrics c_Metrics(c_Font);

      //Consider each item text
      for (sint32 s32_ItItem = 0; s32_ItItem < this->count(); ++s32_ItItem)
      {
         s32_MaxWidth =
            std::max(s32_MaxWidth, static_cast<sint32>(c_Metrics.width(this->itemText(s32_ItItem))));
      }
      //Use combo box size as minimum, otherwise use longest entry
      //+ magic number (some offset to avoid cutting of any text)
      s32_MaxWidth = std::max(s32_MaxWidth + 10L, static_cast<sint32>(this->minimumSizeHint().width()));
      //5: Padding left
      this->view()->setMinimumWidth(static_cast<sintn>(s32_MaxWidth + 5L));
   }
   //Height: Number of items + border
   //31: Item height
   //2: Top & bottom border
   this->view()->setMinimumHeight((this->count() * 31) + 2);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Explicitly ignore events which are globally handled (Won't be forwarded otherwise!)

   \param[in,out] opc_Event Event identification and information

   \created     05.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeCbxTable::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (C_OgeWiUtil::h_CheckGlobalKey(opc_Event) == true)
   {
      opc_Event->ignore();
   }
   else
   {
      QComboBox::keyPressEvent(opc_Event);
   }
}
