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

#include "C_OgeCbxMultiSelect.h"
#include "C_OgeCbxTableBase.h"

#include <QVBoxLayout>
#include <QStylePainter>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
   C_OgeCbxTableBase(opc_Parent),
   mu32_Screenbound(50),
   mpc_PopFrame(new QFrame(NULL, Qt::Popup)),
   mpc_ListWidget(new QListWidget),
   mpc_ListWidgetItem(NULL)
{
   this->SetDisplayText("Not Set");

   // setup the popup list
   this->mpc_PopFrame->setLayout(new QVBoxLayout());
   this->mpc_PopFrame->layout()->addWidget(this->mpc_ListWidget);
   this->mpc_PopFrame->layout()->setContentsMargins(0, 0, 0, 0);

   connect(this->mpc_ListWidget, &QListWidget::itemChanged, this, &C_OgeCbxMultiSelect::m_ScanItemSelect);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxMultiSelect::~C_OgeCbxMultiSelect()
{
   disconnect(this->mpc_ListWidget);
   delete this->mpc_ListWidgetItem;
   delete this->mpc_ListWidget;
   delete this->mpc_PopFrame;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the text to combo box

   \param[in]  oc_Text    Text, which is set
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
/*! \brief  Overwritten paint event slot

   Here: Paint multi select combo box

   \param[in,out]  opc_Event    Event identification and information
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

   \param[in]  orc_Text        Text of the item
   \param[in]  orc_UserData    Data of the item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxMultiSelect::AddItem(const QString & orc_Text, const QVariant & orc_UserData)
{
   this->mpc_ListWidgetItem = new QListWidgetItem(orc_Text);

   this->mpc_ListWidgetItem->setFlags(this->mpc_ListWidgetItem->flags() | Qt::ItemIsUserCheckable);

   if (orc_UserData.toBool())
   {
      this->mpc_ListWidgetItem->setCheckState(Qt::Checked);
   }
   else
   {
      this->mpc_ListWidgetItem->setCheckState(Qt::Unchecked);
   }

   this->mpc_ListWidget->addItem(this->mpc_ListWidgetItem);
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
   if (opc_Item->checkState() == Qt::Checked)
   {
      if ((!this->mc_DisplayText.contains("Not Set")) || (!this->mc_DisplayText.contains("")))
      {
         this->SetDisplayText(this->mc_DisplayText + ", " + opc_Item->text());
      }
      else
      {
         this->SetDisplayText((this->mc_DisplayText.remove(0, this->mc_DisplayText.size())) + opc_Item->text());
      }
   }
   else
   {
      if (this->mc_DisplayText.contains(opc_Item->text() + ", "))
      {
         this->mc_DisplayText = this->mc_DisplayText.remove(opc_Item->text() + ", ");
      }
      else if (!this->mc_DisplayText.contains(", "))
      {
         this->mc_DisplayText = this->mc_DisplayText.remove(opc_Item->text()) + "Not Set";
      }
      else
      {
         this->mc_DisplayText = this->mc_DisplayText.remove(", " + opc_Item->text());
      }
      this->SetDisplayText(this->mc_DisplayText);
   }
}
