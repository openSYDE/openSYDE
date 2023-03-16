//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of all chart data series

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include <QScrollBar>
#include "stwerrors.hpp"

#include "C_SyvDaChaDataSelectorWidget.hpp"
#include "ui_C_SyvDaChaDataSelectorWidget.h"

#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaDataSelectorWidget::C_SyvDaChaDataSelectorWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaChaDataSelectorWidget),
   mpc_Spacer(NULL),
   mu32_ViewIndex(0U),
   mu32_CurrentSelectedIndex(0U),
   me_MeasurementValueState(C_SyvDaChaDataItemWidget::eMEASUREMENT_VAL_DEACTIVATED)
{
   mpc_Ui->setupUi(this);

   // Deactivate custom context menu of scroll bar
   this->mpc_Ui->pc_ScrollAreaChart->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->mpc_Ui->pc_ScrollAreaChart->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Spacer and the Qt memory management
C_SyvDaChaDataSelectorWidget::~C_SyvDaChaDataSelectorWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the index of the current view

   Must be set first

   \param[in]  ou32_ViewIndex    Index of system view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::SetView(const uint32_t ou32_ViewIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific data serie

   \param[in]  ou32_DataPoolElementConfigIndex     Datapool element configuration index
   \param[in]  orc_DataPoolElementId               Datapool element identification
   \param[in]  orc_DisplayName                     Name to display
   \param[in]  oq_Active                           Flag if data serie is active / visible
   \param[in]  orc_Color                           Color of data serie
   \param[in]  orc_Unit                            Unit of current scaling
   \param[in]  oq_Warning                          Flag if a warning for this data element was detected
   \param[in]  oq_Invalid                          Flag if data elment is invalid and invalid placeholder info
                                                   shall be used
   \param[in]  oe_InvalidPlaceholderDataPoolType   Placeholder datapool type of data element in case of
                                                   invalid data element
   \param[in]  orc_ToolTipErrorTextHeading         Heading of tool tip in case of a warning
   \param[in]  orc_ToolTipErrorText                Text of tool tip in case of a warning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::AddDataSerie(const uint32_t ou32_DataPoolElementConfigIndex,
                                                const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                                const QString & orc_DisplayName, const bool oq_Active,
                                                const QColor & orc_Color, const QString & orc_Unit,
                                                const bool oq_Warning, const bool oq_Invalid,
                                                const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType, const QString & orc_ToolTipErrorTextHeading,
                                                const QString & orc_ToolTipErrorText)
{
   C_SyvDaChaDataItemWidget * const pc_DataSerieItem = new C_SyvDaChaDataItemWidget(this->mu32_ViewIndex);

   pc_DataSerieItem->InitWidget(ou32_DataPoolElementConfigIndex, orc_DataPoolElementId, orc_DisplayName,
                                oq_Active, orc_Color, orc_Unit, oq_Warning, oq_Invalid,
                                oe_InvalidPlaceholderDataPoolType,
                                orc_ToolTipErrorTextHeading,
                                orc_ToolTipErrorText);

   pc_DataSerieItem->setParent(this);
   pc_DataSerieItem->SetMeasurementState(this->me_MeasurementValueState);
   this->mpc_Ui->pc_VerticalLayout->addWidget(pc_DataSerieItem);
   connect(pc_DataSerieItem, &C_SyvDaChaDataItemWidget::SigDataItemToggled,
           this, &C_SyvDaChaDataSelectorWidget::SigDataItemToggled);
   connect(pc_DataSerieItem, &C_SyvDaChaDataItemWidget::SigDataItemSelected,
           this, &C_SyvDaChaDataSelectorWidget::m_DataItemSelectedSlot);

   // Add spacer for automatic resizing and positioning
   if (this->mpc_Spacer != NULL)
   {
      this->mpc_Ui->pc_VerticalLayout->removeItem(this->mpc_Spacer);
      delete this->mpc_Spacer;
   }
   this->mpc_Spacer = new QSpacerItem(0, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);
   this->mpc_Ui->pc_VerticalLayout->addSpacerItem(this->mpc_Spacer);
   this->mpc_Ui->pc_VerticalLayout->setStretch(this->mpc_Ui->pc_VerticalLayout->count() - 1, 1);

   // Add widget to map
   this->mc_DataPoolElementsDataItemWidgets.push_back(pc_DataSerieItem);

   if (oq_Active == true)
   {
      // Select the new item
      this->m_DataItemSelected(ou32_DataPoolElementConfigIndex);
   }
} //lint !e429  //no memory leak because of the parent of pc_DataSerieItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current data serie

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification

   \return
   true     data element removed
   false    nothing removed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataSelectorWidget::RemoveDataSerie(const uint32_t ou32_DataPoolElementConfigIndex)
{
   bool q_Return = false;
   uint32_t u32_Counter;

   // Remove the item widget
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      C_SyvDaChaDataItemWidget * const pc_DataSerieItem =
         this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex];

      disconnect(pc_DataSerieItem, &C_SyvDaChaDataItemWidget::SigDataItemToggled,
                 this, &C_SyvDaChaDataSelectorWidget::SigDataItemToggled);
      disconnect(pc_DataSerieItem, &C_SyvDaChaDataItemWidget::SigDataItemSelected,
                 this, &C_SyvDaChaDataSelectorWidget::m_DataItemSelectedSlot);

      this->mpc_Ui->pc_VerticalLayout->removeWidget(pc_DataSerieItem);
      this->mc_DataPoolElementsDataItemWidgets.erase(
         this->mc_DataPoolElementsDataItemWidgets.begin() + ou32_DataPoolElementConfigIndex);

      delete pc_DataSerieItem;

      q_Return = true;
   }

   // Update all indexes of the widgets
   for (u32_Counter = 0U; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
   {
      this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->UpdateIndex(u32_Counter);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of a concrete data serie

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
   \param[in]  orc_Color                        New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::UpdateDataSerieColor(const uint32_t ou32_DataPoolElementConfigIndex,
                                                        const QColor & orc_Color) const
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->UpdateColor(orc_Color);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of a concrete data serie

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
   \param[in]  orc_Value                        New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::UpdateDataSerieValue(const uint32_t ou32_DataPoolElementConfigIndex,
                                                        const QString & orc_Value) const
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->UpdateValue(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the measurement value state

   eMEASUREMENT_VAL_DEACTIVATED:    The last received value will be shown
   eMEASUREMENT_VAL_FIRST           The measurement value of the first cursor will be shown
   eMEASUREMENT_VAL_SECOND          The measurement value of the second cursor will be shown

   \param[in]  oe_MeasurementValueState   Selecting which measurement value has to be shown.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::SetMeasurementState(
   const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueState)
{
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
   {
      this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->SetMeasurementState(oe_MeasurementValueState);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates one of the shown measurement value of the data item

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
   \param[in]  oe_MeasurementValueVariant       Selecting which measurement value has to be set.
                                                eMEASUREMENT_VAL_DEACTIVATED is ignored here
   \param[in]  orc_Value                        New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::UpdateDataSerieMeasurementValue(const uint32_t ou32_DataPoolElementConfigIndex,
                                                                   const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueVariant,
                                                                   const QString & orc_Value)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->UpdateMeasurementValue(
         oe_MeasurementValueVariant, orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error update for data element

   \param[in]  ou32_DataElementIndex   Index of shown datapool element in widget
   \param[in]  orc_ErrorText           Error description
   \param[in]  oq_IsTransmissionError  Flag if transmission error occurred
   \param[in]  oq_ErrorActive          Flag if error is active or should be cleared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::UpdateError(const uint32_t ou32_DataElementIndex, const QString & orc_ErrorText,
                                               const bool oq_IsTransmissionError, const bool oq_ErrorActive) const
{
   if (ou32_DataElementIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataElementIndex]->UpdateError(orc_ErrorText,
                                                                                   oq_IsTransmissionError,
                                                                                   oq_ErrorActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the error state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::ResetError(void) const
{
   uint32_t u32_Index;

   for (u32_Index = 0U; u32_Index < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Index)
   {
      this->mc_DataPoolElementsDataItemWidgets[u32_Index]->ResetError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]  ou32_DataElementIndex   Index of shown datapool element in widget
   \param[in]  os32_Value              Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value)
{
   if (ou32_DataElementIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataElementIndex]->UpdateTransparency(os32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification

   \return
   Element name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaChaDataSelectorWidget::GetDataElementName(const uint32_t ou32_DataPoolElementConfigIndex) const
{
   QString c_Return = "";

   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      c_Return = this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->GetDataElementName();
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification

   \return
   Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaChaDataSelectorWidget::GetDataElementUnit(const uint32_t ou32_DataPoolElementConfigIndex) const
{
   QString c_Return = "";

   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      c_Return = this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->GetDataElementUnit();
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the toggled state if the element is active or not

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification

   \retval   true    Element is enabled
   \retval   false   Element is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataSelectorWidget::GetDataElementToggledState(const uint32_t ou32_DataPoolElementConfigIndex) const
{
   bool q_Return = false;

   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      q_Return =
         this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->GetDataElementToggledState();
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new unit

   long description of function within several lines

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element identification
   \param[in]  orc_DisplayName                  Datapool element display name
   \param[in]  orc_Unit                         Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::SetDataElementUnit(const uint32_t ou32_DataPoolElementConfigIndex,
                                                      const QString & orc_DisplayName, const QString & orc_Unit)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->SetDataElementUnit(orc_DisplayName,
                                                                                                    orc_Unit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the current id of the selected data element

   \param[out]  oru32_DataPoolElementConfigIndex   Current data series id

   \return
   true     data element exists
   false    data element does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataSelectorWidget::GetCurrentDataSerie(uint32_t & oru32_DataPoolElementConfigIndex) const
{
   bool q_Return = false;

   if (this->mc_DataPoolElementsDataItemWidgets.size() > 0)
   {
      oru32_DataPoolElementConfigIndex = this->mu32_CurrentSelectedIndex;
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns if a selector item widget of any data series is on a specific position

   \param[in]   orc_Pos                            Position to check for

   \return
   true     A data element is on the position
   false    No data element is on the position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataSelectorWidget::IsAnyDataSerieOnPosition(const QPoint & orc_Pos) const
{
   bool q_Return = false;
   const QPoint c_PosAdapted = this->mapFromParent(orc_Pos);

   if (this->mc_DataPoolElementsDataItemWidgets.size() > 0)
   {
      uint32_t u32_Counter;

      for (u32_Counter = 0; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
      {
         if (this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->geometry().contains(c_PosAdapted) == true)
         {
            q_Return = true;
            break;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the concrete datapool element

   \param[in]  ou32_DataPoolElementConfigIndex  Current data serie id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::SelectDataSerie(const uint32_t ou32_DataPoolElementConfigIndex)
{
   this->m_DataItemSelected(ou32_DataPoolElementConfigIndex, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Hide tool tip of all items. It is possible that one item does not get the leave event and the tool tip
   would not be closed

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataSelectorWidget::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::Leave)
   {
      uint32_t u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
      {
         this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->HideToolTip();
      }
   }

   return QWidget::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::m_DataItemSelectedSlot(const uint32_t ou32_DataPoolElementConfigIndex)
{
   this->m_DataItemSelected(ou32_DataPoolElementConfigIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataSelectorWidget::m_DataItemSelected(const uint32_t ou32_DataPoolElementConfigIndex,
                                                      const bool oq_SendSignal)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      uint32_t u32_Counter;

      // Select the item
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->SetDataElementSelected(true);

      // Deselect all other items
      for (u32_Counter = 0U; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
      {
         if (u32_Counter != ou32_DataPoolElementConfigIndex)
         {
            this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->SetDataElementSelected(false);
         }
      }

      this->mu32_CurrentSelectedIndex = ou32_DataPoolElementConfigIndex;

      if (oq_SendSignal == true)
      {
         Q_EMIT (this->SigDataItemSelected(ou32_DataPoolElementConfigIndex));
      }
   }
}
