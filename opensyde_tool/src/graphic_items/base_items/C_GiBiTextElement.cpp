//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item (implementation)

   X

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QGraphicsView>

#include "C_GiBiTextElement.h"
#include "C_GiCustomFunctions.h"
#include "C_OgePopUpDialog.h"
#include "C_GtGetText.h"
#include "C_GiSyBaseWidget.h"
#include "C_GiSyTextElementWidget.h"
#include "C_PuiSdDataElement.h"
#include "gitypes.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64 mf64_ActionPointOffsetBoundary = 10.0;

const float64 C_GiBiTextElement::mhf64_MinWidthTextElement = 70.0;
const float64 C_GiBiTextElement::mhf64_MinHeightTextElement = 20.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID    Unique ID
   \param[in]     oq_Editable Flag for editing the content of the text element
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiTextElement::C_GiBiTextElement(const stw_types::uint64 & oru64_ID, const bool oq_Editable,
                                     QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_ID, mhf64_MinWidthTextElement, mhf64_MinHeightTextElement, mf64_ActionPointOffsetBoundary,
                       false, opc_Parent),
   mpc_TextItem(NULL),
   mq_Editable(oq_Editable)
{
   this->m_Init();
   //lint -e{1566}  no memory leak because of the parent of mpc_TextItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID   Unique ID
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiTextElement::C_GiBiTextElement(const uint64 & oru64_ID, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_ID, mhf64_MinWidthTextElement, mhf64_MinHeightTextElement, mf64_ActionPointOffsetBoundary,
                       false, opc_Parent),
   mpc_TextItem(NULL),
   mq_Editable(true)
{
   this->m_Init();
   //lint -e{1566}  no memory leak because of the parent of mpc_TextItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::m_Init()
{
   QFont c_Font;
   //lint -e{1938}  static const is guaranteed preinitialized before main
   const QRectF c_Rect = QRectF(0.0, 0.0,
                                std::max(mhf64_MinWidthTextElement, 1.0),
                                std::max(mhf64_MinHeightTextElement, 1.0));

   this->mpc_TextItem = new C_GiText(c_Rect, this->mq_Editable);

   // Notify the base class about the boundary as biggest item as orientation. Very important!
   this->m_SetBiggestItem(*this->mpc_TextItem);

   connect(this->mpc_TextItem, &C_GiText::SigChangedSize, this,
           &C_GiBiTextElement::m_BiggestItemChanged);

   this->addToGroup(this->mpc_TextItem);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_TEXT_ELEMENT);

   this->setHandlesChildEvents(false);

   // Set default font
   c_Font = mc_STYLE_GUIDE_FONT_REGULAR_16_ITALIC;
   //Convert point to pixel
   c_Font.setPixelSize(c_Font.pointSize());
   this->mpc_TextItem->setFont(c_Font);

   //Mouse cursor
   connect(this->mpc_TextItem, &C_GiText::SigTextInteractionModeStateChanged, this,
           &C_GiBiTextElement::m_HandleTextInteractionMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_TextItem and the Qt memory management
C_GiBiTextElement::~C_GiBiTextElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiBiTextElement::type() const
{
   return msn_GRAPHICS_ITEM_TEXTELEMENT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open style dialog

   \param[in] oq_DarkMode Optional dark mode flag

   \return
   true     Ok was clicked
   false    Cancel was clicked
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiTextElement::OpenStyleDialog(const bool oq_DarkMode)
{
   bool q_Retval;

   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * const pc_Dialog =
      new C_GiSyBaseWidget(*c_New, C_GtGetText::h_GetText("Text Element"), oq_DarkMode);
   C_GiSyTextElementWidget * const pc_SettingsWidget = new C_GiSyTextElementWidget(*pc_Dialog);

   pc_SettingsWidget->SetFontColor(this->GetFontColor());
   pc_SettingsWidget->SetFontStyle(this->GetFontStyle());

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      ApplyStyle(pc_SettingsWidget->GetFontStyle(), pc_SettingsWidget->GetFontColor());
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }

   this->mpc_TextItem->AutoAdaptSize();
   return q_Retval;
} //lint !e429  //no memory leak because of the parent of pc_Dialog and pc_SettingsWidget and the
  //Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   const C_GiBiTextElement * const pc_Item = dynamic_cast<const C_GiBiTextElement * const>(opc_GuidelineItem);

   if (pc_Item != NULL)
   {
      this->ApplyStyle(pc_Item->GetFontStyle(), pc_Item->GetFontColor());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::RestoreDefaultCursor(void)
{
   C_GiBiRectBaseGroup::RestoreDefaultCursor();
   this->mpc_TextItem->setCursor(this->mc_DefaultCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change mouse cursor temporarily

   \param[in] orc_TemporaryCursor New mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   C_GiBiRectBaseGroup::SetTemporaryCursor(orc_TemporaryCursor);
   this->mpc_TextItem->setCursor(static_cast<QCursor>(Qt::IBeamCursor));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set shown text

   \param[in] orc_Text New text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::SetText(const QString & orc_Text)
{
   this->mpc_TextItem->SetText(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set font style

   \param[in] orc_Font New font style configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::SetFontStyle(const QFont & orc_Font)
{
   this->mpc_TextItem->setFont(orc_Font);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set font color

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::SetFontColor(const QColor & orc_Color)
{
   this->mpc_TextItem->setDefaultTextColor(orc_Color);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get displayed text

   \return
   Plain text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiBiTextElement::GetText() const
{
   return this->mpc_TextItem->toPlainText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get font style

   \return
   Font style
*/
//----------------------------------------------------------------------------------------------------------------------
QFont C_GiBiTextElement::GetFontStyle() const
{
   return this->mpc_TextItem->font();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get font color

   \return
   Font color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiBiTextElement::GetFontColor() const
{
   return this->mpc_TextItem->defaultTextColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to apply style and trigger updates

   \param[in] orc_Font  New font
   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::ApplyStyle(const QFont & orc_Font, const QColor & orc_Color)
{
   this->SetFontColor(orc_Color);
   this->SetFontStyle(orc_Font);
   this->mpc_TextItem->AutoAdaptSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Setting the flag if the text element is editable

   \param[in] oq_Editable Flag for editing the content of the text element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::SetEditable(const bool oq_Editable)
{
   this->mq_Editable = oq_Editable;
   this->mpc_TextItem->SetEditable(oq_Editable);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading internal data

   \param[in,out] opc_Data    Text element data
   \param[in]     oq_DarkMode Optional flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::m_LoadTextElementData(const C_PuiBsTextElement * const opc_Data, const bool oq_DarkMode)
{
   this->LoadBasicData(*opc_Data);

   if (oq_DarkMode == true)
   {
      this->SetFontColor(opc_Data->c_UIFontColorDark);
   }
   else
   {
      this->SetFontColor(opc_Data->c_UIFontColorBright);
   }
   this->SetFontStyle(opc_Data->c_UIFontStyle);
   this->SetText(opc_Data->c_UIText);
   this->ApplySizeChange(opc_Data->c_UIPosition, QSizeF(opc_Data->f64_Width, opc_Data->f64_Height));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for updating internal data

   \param[in,out] opc_Data    Text element data
   \param[in]     oq_DarkMode Optional flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::m_UpdateTextElementData(C_PuiBsTextElement * const opc_Data, const bool oq_DarkMode) const
{
   this->UpdateBasicData(*opc_Data);

   if (oq_DarkMode == true)
   {
      opc_Data->c_UIFontColorDark = this->GetFontColor();
   }
   else
   {
      opc_Data->c_UIFontColorBright = this->GetFontColor();
   }
   opc_Data->c_UIFontStyle = this->GetFontStyle();
   opc_Data->c_UIText = this->GetText();
}

//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiBiTextElement::m_ResizeUpdateItems(const float64 of64_DiffWidth, const float64 of64_DiffHeight)
{
   Q_UNUSED(of64_DiffWidth)
   Q_UNUSED(of64_DiffHeight)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden key press event

   \param[in,out] opc_Event Pointer to key event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_F2)) &&
       (this->mq_Editable == true))
   {
      QTextCursor c_Cursor = this->mpc_TextItem->textCursor();

      this->mpc_TextItem->SetTextInteraction(true);

      // Select the entire text
      c_Cursor.select(QTextCursor::Document);
      this->mpc_TextItem->setTextCursor(c_Cursor);
   }
   else
   {
      C_GiBiRectBaseGroup::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse move event slot

   Here: do not allow mouse move when editor mode active

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (this->mpc_TextItem->IsEditModeActive() == false)
   {
      C_GiBiRectBaseGroup::mouseMoveEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   \param[in] oe_Change Indicator what changed
   \param[in] orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiBiTextElement::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   QVariant c_Return = C_GiBiRectBaseGroup::itemChange(oe_Change, orc_Value);

   switch (oe_Change) //lint !e788 //All other cases handled by call of parent
   {
   case ItemSceneHasChanged:
      if (this->scene() != NULL)
      {
         // item was added to scene
         this->mpc_TextItem->installSceneEventFilter(this);
      }
      break;
   case ItemSceneChange:
      if (orc_Value.isNull() == true)
      {
         // item was removed from scene
         this->mpc_TextItem->removeSceneEventFilter(this);
      }
      break;
   default:
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten scene event filter event slot

   Prevent single click the text item to select it with one click.

   \param[in]     opc_Watched Actual event receiver
   \param[in,out] opc_Event   Event identification and information

   \return
   true     Event will be ignored
   false    Event will be forwarded normally
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiTextElement::sceneEventFilter(QGraphicsItem * const opc_Watched, QEvent * const opc_Event)
{
   bool q_Return = false;

   if ((opc_Watched == this->mpc_TextItem) &&
       (this->mpc_TextItem->IsEditModeActive() == false))
   {
      QGraphicsSceneMouseEvent * const pc_MouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(opc_Event);

      switch (opc_Event->type()) //lint !e788  not all enum constants are necessary here
      {
      case QEvent::GraphicsSceneMousePress:
         C_GiBiRectBaseGroup::mousePressEvent(pc_MouseEvent);
         q_Return = true;
         break;
      case QEvent::GraphicsSceneMouseRelease:
         C_GiBiRectBaseGroup::mouseReleaseEvent(pc_MouseEvent);
         q_Return = true;
         break;
      case QEvent::GraphicsSceneMouseMove:
         C_GiBiRectBaseGroup::mouseMoveEvent(pc_MouseEvent);
         q_Return = true;
         break;
      default:
         break;
      }
   }
   else
   {
      C_GiBiRectBaseGroup::sceneEventFilter(opc_Watched, opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  React to text interaction mode change

   \param[in] orq_On Flag if text interaction mode is currently active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiTextElement::m_HandleTextInteractionMode(const bool & orq_On)
{
   Q_EMIT this->SigTextInteractionModeStateChanged(orq_On);

   if (orq_On == true)
   {
      this->SetTemporaryCursor(static_cast<QCursor>(Qt::ArrowCursor));
   }
   else
   {
      this->RestoreDefaultCursor();
   }
}
