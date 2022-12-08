//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with multi-selection functionality

   Combo box with multi-selection functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QVBoxLayout>
#include <QStylePainter>
#include <QApplication>
#include <QDesktopWidget>

#include "stwtypes.hpp"

#include "C_GtGetText.hpp"
#include "C_OgeCbxMultiSelect.hpp"
#include "C_OgeCbxTableBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
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
C_OgeCbxMultiSelect::C_OgeCbxMultiSelect(QWidget * const opc_Parent) :
   C_OgeCbxToolTipBase(opc_Parent),
   mpc_PopFrame(new QFrame(this, Qt::Popup)),
   mpc_ListWidget(new QListWidget),
   mq_IsShown(false)
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
   connect(this->mpc_ListWidget, &QListWidget::clicked, this, &C_OgeCbxMultiSelect::m_ListWidgetItemClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ListWidget (use addWidget) and the Qt memory management
C_OgeCbxMultiSelect::~C_OgeCbxMultiSelect()
{
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
   for (int32_t s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); ++s32_Counter)
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
/*! \brief  Overwritten mouse press event slot

   Here: Mouse click to expand or collapse

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::mousePressEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (this->mq_IsShown == false)
   {
      this->showPopup();
   }
   else
   {
      this->hidePopup();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show the popup from the combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::showPopup()
{
   QRect c_Rect;
   const int32_t s32_ListHeight = (this->mpc_ListWidget->count() * this->mpc_ListWidget->sizeHintForRow(0)) + 2;
   const QRect c_ScreenRect = QApplication::desktop()->screenGeometry(this);
   const QPoint c_Above = this->mapToGlobal(QPoint(0, 0));
   const QPoint c_Below = this->mapToGlobal(QPoint(0, this->geometry().height()));
   const int32_t s32_BelowHeight = std::min(abs(c_ScreenRect.bottom() - c_Below.y()), s32_ListHeight);
   const int32_t s32_AboveHeight = std::min(abs(c_Above.y() - c_ScreenRect.y()), s32_ListHeight);

   // give the list widget a small minimum (important if only 2 or less entries exist)
   this->mpc_ListWidget->setMinimumHeight(1);

   // if we use below or above
   if ((s32_BelowHeight == s32_ListHeight) || (s32_BelowHeight > s32_AboveHeight))
   {
      c_Rect.setTopLeft(c_Below);
      c_Rect.setWidth(this->geometry().width());
      c_Rect.setHeight(s32_ListHeight);
   }
   else
   {
      c_Rect.setTopLeft(c_Above - QPoint(0, s32_AboveHeight));
      c_Rect.setWidth(this->geometry().width());
      c_Rect.setHeight(s32_ListHeight);
   }

   // set popup geometry
   this->mpc_PopFrame->setGeometry(c_Rect);
   this->mpc_PopFrame->raise();
   this->mpc_PopFrame->show();

   // remember
   this->mq_IsShown = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide the pop up from the combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::hidePopup()
{
   this->mpc_PopFrame->hide();

   this->mq_IsShown = false;
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
} //lint !e429  //no memory leak because of the parent of pc_ListWidgetItem (use addItem) and the Qt memory management

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
   for (int32_t s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); s32_Counter++)
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
      for (int32_t s32_It = 0; s32_It < orc_Strings.size(); ++s32_It)
      {
         this->AddItem(orc_Strings.at(s32_It), orc_Values.at(s32_It));
      }
      this->m_UpdateDisplayName();
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
   this->m_UpdateDisplayName();

   // inform about value changed
   Q_EMIT (SigValueChanged(opc_Item->text(), opc_Item->checkState() == Qt::Checked));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Combo box and tool tip text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::m_UpdateDisplayName(void)
{
   bool q_Found = false;
   QString c_Display;
   QString c_ToolTip;

   // search for items are checked
   for (int32_t s32_Counter = 0; s32_Counter < this->mpc_ListWidget->count(); s32_Counter++)
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
               c_ToolTip += ",\n";
            }
            c_Display += pc_Item->text();
            c_ToolTip += pc_Item->text();
         }
      }
   }

   // no checked item found
   if (q_Found == false)
   {
      c_Display = C_GtGetText::h_GetText("<none>");
      c_ToolTip = C_GtGetText::h_GetText("Nothing selected");
   }

   // set the item names to combo box
   this->SetDisplayText(c_Display);
   this->SetToolTipInformation(C_GtGetText::h_GetText("Selected Elements:"), c_ToolTip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check or uncheck clicked item

   If an item checked, the text of this item is set to the combo box text.
   If an item unchecked, the text of this item removed from the combo box text

   \param[in]  orc_ModelIndex    Index of the list widget model
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::m_ListWidgetItemClicked(const QModelIndex & orc_ModelIndex)
{
   QListWidgetItem * const pc_Item = this->mpc_ListWidget->item(orc_ModelIndex.row());

   if (pc_Item != NULL)
   {
      if (pc_Item->checkState() == Qt::Checked)
      {
         pc_Item->setCheckState(Qt::Unchecked);
      }
      else
      {
         pc_Item->setCheckState(Qt::Checked);
      }
      this->m_ScanItemSelect(pc_Item);
   }
}
