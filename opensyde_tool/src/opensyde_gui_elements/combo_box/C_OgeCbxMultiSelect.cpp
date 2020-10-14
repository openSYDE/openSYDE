//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with multi-selection functionality

   Combo box with multi-selection functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QVBoxLayout>
#include <QStylePainter>

#include "stwtypes.h"

#include "C_GtGetText.h"
#include "C_OgeCbxMultiSelect.h"
#include "C_OgeCbxTableBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
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
C_OgeCbxMultiSelect::C_OgeCbxMultiSelect(QWidget * const opc_Parent) :
   QComboBox(opc_Parent),
   mu32_Screenbound(50),
   mpc_PopFrame(new QFrame(this, Qt::Popup)),
   mpc_ListWidget(new QListWidget)
{
   this->SetDisplayText(C_GtGetText::h_GetText("<none>"));

   //Handle pop up interaction even with parent
   this->mpc_PopFrame->setAttribute(Qt::WA_WindowPropagation);
   this->mpc_PopFrame->setAttribute(Qt::WA_X11NetWmWindowTypeCombo);
   // setup the popup list
   this->mpc_PopFrame->setLayout(new QVBoxLayout());
   this->mpc_PopFrame->layout()->addWidget(this->mpc_ListWidget);
   this->mpc_PopFrame->layout()->setContentsMargins(0, 0, 0, 0);

   connect(this->mpc_ListWidget, &QListWidget::itemClicked, this, &C_OgeCbxMultiSelect::m_ScanItemSelect);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxMultiSelect::~C_OgeCbxMultiSelect()
{
   //lint -e{1540}  no memory leak because of the parent of mpc_ListWidget (use addWidget) and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the text to combo box

   \param[in]  oc_Text  Text, which is set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::SetDisplayText(const QString oc_Text)
{
   this->mc_DisplayText = oc_Text;
   this->repaint();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the text from combo box

   \return
   String, text from combo box
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeCbxMultiSelect::GetDisplayText() const
{
   return this->mc_DisplayText;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all items from combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::RemoveAllItems()
{
   this->mpc_ListWidget->clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get values as bit array

   \return
   Values as bit array
*/
//----------------------------------------------------------------------------------------------------------------------
QBitArray C_OgeCbxMultiSelect::GetValuesAsBitArray(void) const
{
   QBitArray c_Retval;

   c_Retval.resize(this->mpc_ListWidget->count());
   for (stw_types::sint32 s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); ++s32_Counter)
   {
      // get one item from list widget
      const QListWidgetItem * const pc_Item = this->mpc_ListWidget->item(s32_Counter);

      if (pc_Item != NULL)
      {
         if (pc_Item->checkState() == Qt::Checked)
         {
            c_Retval.setBit(s32_Counter, true);
         }
         else
         {
            c_Retval.setBit(s32_Counter, false);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten paint event slot

   Here: Paint multi select combo box

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QStylePainter c_Painter(this);
   // draw the combo box frame, focusrect and selected etc.
   QStyleOptionComboBox c_StyleOption;

   // Initializes the state, direction, rect, palette, and fontMetrics member variables based on the specified widget.
   // This is a convenience function; the member variables can also be initialized manually.
   c_StyleOption.initFrom(this);
   c_StyleOption.currentText = this->mc_DisplayText;
   c_Painter.drawComplexControl(QStyle::CC_ComboBox, c_StyleOption);
   // draw the icon and text
   c_Painter.drawControl(QStyle::CE_ComboBoxLabel, c_StyleOption);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show the popup from the combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::showPopup()
{
   QRect c_Rect = QRect(geometry());

   // get the two possible list points and height
   QPoint c_Below = this->mapToGlobal(QPoint(0, c_Rect.height()));

   // first activate it with height 1px to get all the items initialized
   QRect c_Rect2;

   c_Rect2.setTopLeft(c_Below);
   c_Rect2.setWidth(c_Rect.width());
   c_Rect.setHeight(1);

   // determine rect
   c_Rect2.setTopLeft(c_Below);
   c_Rect2.setHeight((this->mpc_ListWidget->count() * this->mpc_ListWidget->sizeHintForRow(0)) + 4);

   this->mpc_PopFrame->setGeometry(c_Rect2);
   this->mpc_PopFrame->raise();
   this->mpc_PopFrame->show();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide the pop up from the combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::hidePopup()
{
   this->mpc_PopFrame->hide();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add an item to the combo box

   Add an item to the list widget

   \param[in]  orc_Text       Text of the item
   \param[in]  orc_UserData   Data of the item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::AddItem(const QString & orc_Text, const QVariant & orc_UserData)
{
   QListWidgetItem * const pc_ListWidgetItem = new QListWidgetItem(orc_Text);

   pc_ListWidgetItem->setFlags(pc_ListWidgetItem->flags() | Qt::ItemIsUserCheckable);

   if (orc_UserData.toBool())
   {
      pc_ListWidgetItem->setCheckState(Qt::Checked);
   }
   else
   {
      pc_ListWidgetItem->setCheckState(Qt::Unchecked);
   }

   this->mpc_ListWidget->addItem(pc_ListWidgetItem);
   //lint -e{429}  no memory leak because of the parent of pc_ListWidgetItem (use addItem) and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the check state to item

   Set the check state "checked" to the items which are checked in the data storage

   \param[in]  orc_DisplayName   Name of the item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::SetItem(const QString & orc_DisplayName)
{
   QString c_Display;

   if (this->mc_DisplayText.contains(C_GtGetText::h_GetText("<none>")))
   {
      this->mc_DisplayText = "";
   }
   // search for the item
   for (stw_types::sint32 s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); s32_Counter++)
   {
      // get one item from list widget
      QListWidgetItem * const pc_Item = this->mpc_ListWidget->item(s32_Counter);

      if (pc_Item != NULL)
      {
         if (pc_Item->text().toStdString().c_str() == orc_DisplayName)
         {
            pc_Item->setCheckState(Qt::Checked);
            // set display text
            if (!this->mc_DisplayText.isEmpty())
            {
               c_Display += ", ";
            }
            c_Display += pc_Item->text();
         }
      }
   }

   // set the item names to combo box
   this->SetDisplayText(this->GetDisplayText() + c_Display);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize from generic types

   \param[in]  orc_Strings    Strings
   \param[in]  orc_Values     Values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::Init(const QStringList & orc_Strings, const QBitArray & orc_Values)
{
   this->RemoveAllItems();
   if (orc_Strings.size() == orc_Values.size())
   {
      for (sintn sn_It = 0; sn_It < orc_Strings.size(); ++sn_It)
      {
         this->AddItem(orc_Strings.at(sn_It), orc_Values.at(sn_It));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check, which item is selected and set it so the combo box text

   If an item checked, the text of this item is set to the combo box text.
   If an item unchecked, the text of this item removed from the combo box text

   \param[in]  opc_Item    Item, which is checked/unchecked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::m_ScanItemSelect(const QListWidgetItem * const opc_Item)
{
   bool q_Found = false;
   QString c_Display;

   // search for items are checked
   for (stw_types::sint32 s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); s32_Counter++)
   {
      // get one item from list widget
      QListWidgetItem * const pc_Item = this->mpc_ListWidget->item(s32_Counter);

      if (pc_Item != NULL)
      {
         if (pc_Item->checkState() == Qt::Checked)
         {
            // a checked item is found
            q_Found = true;

            // set display text
            if (!c_Display.isEmpty())
            {
               c_Display += ", ";
            }
            c_Display += pc_Item->text();
         }
      }
   }

   // no checked item found
   if (q_Found == false)
   {
      c_Display = C_GtGetText::h_GetText("<none>");
   }

   // set the item names to combo box
   this->SetDisplayText(c_Display);

   // inform about value changed
   Q_EMIT (SigValueChanged(opc_Item->text(), opc_Item->checkState() == Qt::Checked));
}
