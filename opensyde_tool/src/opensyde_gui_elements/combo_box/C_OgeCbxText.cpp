//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with text (implementation)

   Custom combo box with text

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeCbxText.hpp"
#include "C_OgeWiUtil.hpp"

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
C_OgeCbxTextLineEdit::C_OgeCbxTextLineEdit(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "LineEditError", false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxText::C_OgeCbxText(QWidget * const opc_Parent) :
   C_OgeCbxIconOnly(opc_Parent),
   mc_TemporaryText(""),
   ms32_TemporaryIndex(-1),
   mq_Error(false),
   mq_SuppressHide(false)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "LineEditError", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Valid", true);

   this->setContextMenuPolicy(Qt::NoContextMenu);

   connect(this, static_cast<void (QComboBox::*)(const QString &)>(&C_OgeCbxText::textActivated), this,
           &C_OgeCbxText::m_Activated);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a temporary text for showing on combo box selection, but not in selection list

   \param[in]  orc_Text    Text for showing
   \param[in]  oq_Error    Error flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxText::SetTemporaryText(const QString & orc_Text, const bool oq_Error)
{
   if (this->mc_TemporaryText != "")
   {
      // remove the old text
      this->removeItem(this->ms32_TemporaryIndex);
      this->ms32_TemporaryIndex = -1;
   }

   this->mc_TemporaryText = orc_Text;

   if (this->mc_TemporaryText != "")
   {
      this->ms32_TemporaryIndex = this->findText(this->mc_TemporaryText);
      if (this->ms32_TemporaryIndex < 0)
      {
         // add the new temporary text if it is not here yet
         this->addItem(this->mc_TemporaryText);
         this->ms32_TemporaryIndex = this->findText(this->mc_TemporaryText);
      }
      this->setCurrentIndex(this->ms32_TemporaryIndex);
   }

   this->mq_Error = oq_Error;

   // Adapt the original valid stylesheet property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Valid", !this->mq_Error);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show popup event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxText::showPopup(void)
{
   if (this->mc_TemporaryText != "")
   {
      C_OgeCbxTextLineEdit * const pc_Edit = new C_OgeCbxTextLineEdit();
      this->SuppressHide(); // suppress next hidePopup

      // In case of showing an error, we need the stylesheet properties of the concrete line edit
      // The QComboBox would overwrite the property of the line edit
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "LineEditError", this->mq_Error);
      C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Edit, "LineEditError", this->mq_Error);
      // Deactivate the original invalid stylesheet property. Show only the line edit as invalid.
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Valid", true);

      this->setEditable(true);

      // Remove the temporary item. When showing all items in the combo box list, the temporary item shall not be showed
      this->removeItem(this->ms32_TemporaryIndex);

      this->setLineEdit(pc_Edit);
      this->lineEdit()->setReadOnly(true);
      this->lineEdit()->setText(this->mc_TemporaryText);
   }

   QComboBox::showPopup();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide popup event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxText::hidePopup(void)
{
   if (mq_SuppressHide == false)
   {
      if (this->mc_TemporaryText != "")
      {
         this->ms32_TemporaryIndex = this->findText(this->mc_TemporaryText);
         if (this->ms32_TemporaryIndex < 0)
         {
            // Add the item again if it was not added already again
            this->addItem(this->mc_TemporaryText);
            this->ms32_TemporaryIndex = this->findText(this->mc_TemporaryText);
         }
         this->setCurrentIndex(this->ms32_TemporaryIndex);

         this->setEditable(false);

         C_OgeWiUtil::h_ApplyStylesheetProperty(this, "LineEditError", false);
         // Adapt the valid stylesheet property again
         C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Valid", !this->mq_Error);
      }
      QComboBox::hidePopup();
   }
   // if suppress hide is true, do not hide the pop up (because in this case somebody wants it to be shown)
   else
   {
      mq_SuppressHide = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxText::m_Activated(const QString & orc_Text)
{
   // call extra hide to make sure popup gets hidden
   this->hidePopup();

   // Was the item changed to a valid item?
   if ((this->mc_TemporaryText != "") &&
       (orc_Text != this->mc_TemporaryText))
   {
      // Change the selection to the new item
      this->setCurrentText(orc_Text);

      // Remove the temporary text and its error
      this->removeItem(this->ms32_TemporaryIndex);
      this->mc_TemporaryText = "";
      this->ms32_TemporaryIndex = -1;

      if (this->mq_Error == true)
      {
         Q_EMIT this->SigErrorFixed();
         this->mq_Error = false;
      }

      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Valid", true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Suppress call to hidePopup() if the LineEdit is clicked.

   The combo box interprets a click on the LineEdit as "outside the box" and
   therefore calls hidePopup(). This is not wanted, since the pop up should
   open when the text is clicked.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxText::SuppressHide()
{
   mq_SuppressHide = true;
}
