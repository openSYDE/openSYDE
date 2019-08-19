//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of all chart data series

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include <QScrollBar>
#include "stwerrors.h"

#include "C_SyvDaItChartDataSelectorWidget.h"
#include "ui_C_SyvDaItChartDataSelectorWidget.h"

#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent                 Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItChartDataSelectorWidget::C_SyvDaItChartDataSelectorWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItChartDataSelectorWidget),
   mpc_Spacer(NULL),
   mu32_ViewIndex(0U),
   mu32_CurrentSelectedIndex(0U)
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
C_SyvDaItChartDataSelectorWidget::~C_SyvDaItChartDataSelectorWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_Spacer and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the index of the current view

   Must be set first

   \param[in]     ou32_ViewIndex             Index of system view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::SetView(const uint32 ou32_ViewIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a specific data serie

   \param[in]     ou32_DataPoolElementConfigIndex         Datapool element configuration index
   \param[in]     orc_DataPoolElementId                   Datapool element identification
   \param[in]     orc_DisplayName                         Name to display
   \param[in]     oq_Active                               Flag if data serie is active / visible
   \param[in]     orc_Color                               Color of data serie
   \param[in]     orc_Unit                                Unit of current scaling
   \param[in]     oq_Warning                              Flag if a warning for this data element was detected
   \param[in]     oq_Invalid                              Flag if data elment is invalid and invalid placeholder info
                                                          shall be used
   \param[in]     oe_InvalidPlaceholderDataPoolType       Placeholder datapool type of data element in case of
                                                          invalid data element
   \param[in]     orc_ToolTipErrorTextHeading             Heading of tool tip in case of a warning
   \param[in]     orc_ToolTipErrorText                    Text of tool tip in case of a warning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::AddDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                                    const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                                    const QString & orc_DisplayName, const bool oq_Active,
                                                    const QColor & orc_Color, const QString & orc_Unit,
                                                    const bool oq_Warning, const bool oq_Invalid,
                                                    const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType, const QString & orc_ToolTipErrorTextHeading,
                                                    const QString & orc_ToolTipErrorText)
{
   C_SyvDaItChartDataItemWidget * pc_DataSerieItem = new C_SyvDaItChartDataItemWidget(this->mu32_ViewIndex);

   pc_DataSerieItem->InitWidget(ou32_DataPoolElementConfigIndex, orc_DataPoolElementId, orc_DisplayName,
                                oq_Active, orc_Color, orc_Unit, oq_Warning, oq_Invalid,
                                oe_InvalidPlaceholderDataPoolType,
                                orc_ToolTipErrorTextHeading,
                                orc_ToolTipErrorText);

   pc_DataSerieItem->setParent(this);
   this->mpc_Ui->pc_VerticalLayout->addWidget(pc_DataSerieItem);
   connect(pc_DataSerieItem, &C_SyvDaItChartDataItemWidget::SigDataItemToggled,
           this, &C_SyvDaItChartDataSelectorWidget::SigDataItemToggled);
   connect(pc_DataSerieItem, &C_SyvDaItChartDataItemWidget::SigDataItemSelected,
           this, &C_SyvDaItChartDataSelectorWidget::m_DataItemSelectedSlot);

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

   //lint -e{429}  no memory leak because of the parent of pc_DataSerieItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes the current data serie

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \return
   true     data element removed
   false    nothing removed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartDataSelectorWidget::RemoveDataSerie(const uint32 ou32_DataPoolElementConfigIndex)
{
   bool q_Return = false;
   uint32 u32_Counter;

   // Remove the item widget
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      C_SyvDaItChartDataItemWidget * const pc_DataSerieItem =
         this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex];

      disconnect(pc_DataSerieItem, &C_SyvDaItChartDataItemWidget::SigDataItemToggled,
                 this, &C_SyvDaItChartDataSelectorWidget::SigDataItemToggled);
      disconnect(pc_DataSerieItem, &C_SyvDaItChartDataItemWidget::SigDataItemSelected,
                 this, &C_SyvDaItChartDataSelectorWidget::m_DataItemSelectedSlot);

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

   \param[in]     ou32_DataPoolElementConfigIndex         Datapool element configuration index
   \param[in]     orc_Value                               New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::UpdateDataSerieValue(const uint32 ou32_DataPoolElementConfigIndex,
                                                            const QString & orc_Value) const
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataPoolElementConfigIndex]->UpdateValue(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error update for data element

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] orc_ErrorText                   Error description
   \param[in] orq_IsTransmissionError         Flag if transmission error occurred
   \param[in] oq_ErrorActive                  Flag if error is active or should be cleared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::UpdateError(const uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
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
void C_SyvDaItChartDataSelectorWidget::ResetError(void) const
{
   uint32 u32_Index;

   for (u32_Index = 0U; u32_Index < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Index)
   {
      this->mc_DataPoolElementsDataItemWidgets[u32_Index]->ResetError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] osn_Value                       Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   if (ou32_DataElementIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      this->mc_DataPoolElementsDataItemWidgets[ou32_DataElementIndex]->UpdateTransparence(osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \return
   Element name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItChartDataSelectorWidget::GetDataElementName(const uint32 ou32_DataPoolElementConfigIndex) const
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

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \return
   Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItChartDataSelectorWidget::GetDataElementUnit(const uint32 ou32_DataPoolElementConfigIndex) const
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

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \retval   true    Element is enabled
   \retval   false   Element is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartDataSelectorWidget::GetDataElementToggledState(const uint32 ou32_DataPoolElementConfigIndex) const
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

   \param[in]     orc_DataPoolElementId   Datapool element identification
   \param[in]     orc_DisplayName         Datapool element display name
   \param[in]     orc_Unit                Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::SetDataElementUnit(const uint32 ou32_DataPoolElementConfigIndex,
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

   \param[out]   orc_DataPoolElementId     Current data serie id

   \return
   true     data element exists
   false    data element does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartDataSelectorWidget::GetCurrentDataSerie(uint32 & oru32_DataPoolElementConfigIndex) const
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
/*! \brief   Selects the concrete datapool element

   \param[in]   orc_DataPoolElementId     Current data serie id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::SelectDataSerie(const uint32 ou32_DataPoolElementConfigIndex)
{
   this->m_DataItemSelected(ou32_DataPoolElementConfigIndex, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Hide tool tip of all items. It is possible that one item does not get the leave event and the tool tip
   would not be closed

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartDataSelectorWidget::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::Leave)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->mc_DataPoolElementsDataItemWidgets.size(); ++u32_Counter)
      {
         this->mc_DataPoolElementsDataItemWidgets[u32_Counter]->HideToolTip();
      }
   }

   return QWidget::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::m_DataItemSelectedSlot(const stw_types::uint32 ou32_DataPoolElementConfigIndex)
{
   this->m_DataItemSelected(ou32_DataPoolElementConfigIndex, true);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataSelectorWidget::m_DataItemSelected(const uint32 ou32_DataPoolElementConfigIndex,
                                                          const bool oq_SendSignal)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataItemWidgets.size())
   {
      uint32 u32_Counter;

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
         Q_EMIT this->SigDataItemSelected(ou32_DataPoolElementConfigIndex);
      }
   }
}
