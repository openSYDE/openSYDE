//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sizeable variant of QGraphicsTextItem (implementation)

   It offers an editable text when double click

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>

#include "stwtypes.hpp"
#include "C_GiText.hpp"
#include "gitypes.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Rect     Initial size of the item
   \param[in]     oq_Editable  Flag for editing the content of the text element
   \param[in,out] opc_Parent   Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiText::C_GiText(const QRectF & orc_Rect, const bool oq_Editable, QGraphicsItem * const opc_Parent) :
   QGraphicsTextItem("", opc_Parent),
   C_GiBiSizeableItem(orc_Rect.width(), orc_Rect.height()),
   mq_Editable(oq_Editable)
{
   setFlag(ItemIsFocusable);
   setTextInteractionFlags(Qt::NoTextInteraction);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiText::~C_GiText(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiText::type(void) const
{
   return ms32_GRAPHICS_ITEM_TEXT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the drawing of the rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::Redraw(void)
{
   this->setTextWidth(this->f64_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bounding rectangle (estimate of the area painted by the item)

   \return
   bounding rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiText::boundingRect(void) const
{
   return QRectF(0.0, 0.0, this->f64_Width, this->f64_Height);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setting the flag if the text element is editable

   \param[in]     oq_Editable    Flag for editing the content of the text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::SetEditable(const bool oq_Editable)
{
   this->mq_Editable = oq_Editable;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the new text and set the new necessary size for the text

   \param[in]  orc_Text    New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::SetText(const QString & orc_Text)
{
   this->setPlainText(orc_Text);
   this->AutoAdaptSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the new text and set the new necessary size for the text

   \return
   true     Text is editable
   false    Text is not editable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiText::IsEditModeActive() const
{
   bool q_Return = false;

   if (this->textInteractionFlags() == static_cast<int32_t>(Qt::TextEditorInteraction))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the size of the item depends on the size of the text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::AutoAdaptSize(void)
{
   // the text changed probably. update the size
   this->f64_Width = QGraphicsTextItem::boundingRect().width();
   this->f64_Height = QGraphicsTextItem::boundingRect().height();

   Q_EMIT this->SigChangedSize();
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiText::SetTextInteraction(const bool oq_On, const bool oq_SelectAll)
{
   if ((oq_On == true) &&
       (this->textInteractionFlags() == static_cast<int32_t>(Qt::NoTextInteraction)))
   {
      // switch on editor mode:
      this->setTextInteractionFlags(Qt::TextEditorInteraction);
      this->setFlag(ItemIsSelectable, true);
      // manually do what a mouse click would do else:
      this->setFocus(Qt::MouseFocusReason); // this gives the item keyboard focus
      this->setSelected(true);              // this ensures that itemChange() gets called when we click out of the item
      if (oq_SelectAll == true)             // option to select the whole text (e.g. after creation of the TextItem)
      {
         QTextCursor c_Curosr = this->textCursor();
         c_Curosr.select(QTextCursor::Document);
         this->setTextCursor(c_Curosr);
      }
      Q_EMIT this->SigTextInteractionModeStateChanged(oq_On);
   }
   else if ((oq_On == false) &&
            (textInteractionFlags() == static_cast<int32_t>(Qt::TextEditorInteraction)))
   {
      // turn off editor mode:
      this->setTextInteractionFlags(Qt::NoTextInteraction);
      this->setFlag(ItemIsSelectable, false);
      // deselect text (else it keeps gray shade):
      QTextCursor c_Curosr = this->textCursor();
      c_Curosr.clearSelection();
      this->setTextCursor(c_Curosr);
      this->clearFocus();
      Q_EMIT this->SigTextInteractionModeStateChanged(oq_On);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event

   Adjust size of item.

   \param[in,out] opc_Event  Pointer to key event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((C_Uti::h_CheckKeyModifier(opc_Event->modifiers(),
                                  Qt::ControlModifier) == true) &&
       (opc_Event->key() == static_cast<int32_t>(Qt::Key_V)))
   {
      const QClipboard * const pc_ClipBoard = QApplication::clipboard();
      QTextCursor c_Cursor = this->textCursor();
      c_Cursor.insertText(pc_ClipBoard->text());
   }
   else
   {
      QGraphicsTextItem::keyPressEvent(opc_Event);
   }

   this->AutoAdaptSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Activate the editor mode

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (this->mq_Editable == true)
   {
      if (this->textInteractionFlags() == static_cast<int32_t>(Qt::TextEditorInteraction))
      {
         // if editor mode is already on: pass double click events on to the editor:
         QGraphicsTextItem::mouseDoubleClickEvent(opc_Event);
      }
      else
      {
         QGraphicsSceneMouseEvent * pc_MouseEvent;

         // if editor mode is off:
         // 1. turn editor mode on and set selected and focused:
         this->SetTextInteraction(true);

         // 2. send a single click to this QGraphicsTextItem (this will set the cursor to the mouse position):
         // create a new mouse event with the same parameters as evt
         pc_MouseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
         pc_MouseEvent->setButton(opc_Event->button());
         pc_MouseEvent->setPos(opc_Event->pos());
         QGraphicsTextItem::mousePressEvent(pc_MouseEvent);
         delete pc_MouseEvent; // don't forget to delete the event
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten itemChange event slot

   \param[in]  oe_Change  Indicator what changed
   \param[in]  orc_Value  Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiText::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   if ((oe_Change == QGraphicsItem::ItemSelectedChange) &&
       (this->textInteractionFlags() != static_cast<int32_t>(Qt::NoTextInteraction)) &&
       (orc_Value.toBool() == false))
   {
      // item received SelectedChange event AND is in editor mode AND is about to be deselected:
      this->SetTextInteraction(false); // leave editor mode
   }
   return QGraphicsTextItem::itemChange(oe_Change, orc_Value);
}
