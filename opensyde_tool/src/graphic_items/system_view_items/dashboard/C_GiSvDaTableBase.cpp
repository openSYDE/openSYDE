//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard table item (implementation)

   Class for system view dashboard table item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include <QGraphicsView>

#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SyvDaPeDataElementBrowse.hpp"
#include "C_SyvDaPeUpdateModeConfiguration.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]      oru64_Id               Unique ID
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaTableBase::C_GiSvDaTableBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                     const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                     QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTABLE,
                         C_SyvDaItTaModel::hu32_MAX_ELEMENTS, oru64_Id, 50.0, 25.0, 100.0, 50.0, false, true,
                         opc_Parent),
   mpc_AddDataElement(NULL),
   mpc_AddSeperator(NULL),
   mpc_ConfigDataElement(NULL),
   mpc_ConfigSeperator(NULL),
   mpc_RemoveDataElement(NULL),
   mpc_MoveUpDataElement(NULL),
   mpc_MoveDownDataElement(NULL),
   mpc_MiscSeperator(NULL)
{
   this->mpc_TableWidget = new C_SyvDaItTaView(this);
   this->mpc_Widget->SetWidget(this->mpc_TableWidget);

   connect(this->mpc_TableWidget, &C_SyvDaItTaView::SigTriggerEdit, this, &C_GiSvDaTableBase::EditElementProperties);
   connect(this->mpc_TableWidget, &C_SyvDaItTaView::SigMoveDataElementDown, this,
           &C_GiSvDaTableBase::m_MoveDataElementDown);
   connect(this->mpc_TableWidget, &C_SyvDaItTaView::SigMoveDataElementUp, this,
           &C_GiSvDaTableBase::m_MoveDataElementUp);
   connect(this->mpc_TableWidget, &C_SyvDaItTaView::SigRemoveDataElement, this,
           &C_GiSvDaTableBase::m_RemoveDataElement);
   connect(this->mpc_TableWidget, &C_SyvDaItTaView::SigAddDataElement, this,
           &C_GiSvDaTableBase::m_AddNewDataElement);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Either handled by Qt parent handling or not owned by this class in the first place
C_GiSvDaTableBase::~C_GiSvDaTableBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaTableBase::type(void) const
{
   return ms32_GRAPHICS_ITEM_DB_TABLE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Style       New style type
   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->SetDisplayStyle(oe_Style, oq_DarkMode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_GiSvDaTableBase::ReInitializeSize(void)
{
   //Required by interface
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::LoadData(void)
{
   tgl_assert(this->ms32_Index >= 0);
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      tgl_assert(pc_View != NULL);
      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         tgl_assert(pc_Dashboard != NULL);
         if (pc_Dashboard != NULL)
         {
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32_t>(this->ms32_Index));
            tgl_assert(pc_Box != NULL);
            if (pc_Box != NULL)
            {
               this->LoadSvBasicData(*pc_Box);
               if (this->mpc_TableWidget != NULL)
               {
                  this->mpc_TableWidget->UpdateStaticValues();
                  this->mpc_TableWidget->SetCurrentColumnWidths(pc_Box->c_ColumnWidth);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::UpdateData(void)
{
   tgl_assert(this->ms32_Index >= 0);
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      tgl_assert(pc_View != NULL);
      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         tgl_assert(pc_Dashboard != NULL);
         if (pc_Dashboard != NULL)
         {
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32_t>(this->ms32_Index));
            tgl_assert(pc_Box != NULL);
            if (pc_Box != NULL)
            {
               C_PuiSvDbTable c_Box = *pc_Box;
               this->UpdateSvBasicData(c_Box, true);

               if (this->mpc_TableWidget != NULL)
               {
                  c_Box.c_ColumnWidth = this->mpc_TableWidget->GetCurrentColumnWidths();
               }
               tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                              this->mu32_DashboardIndex,
                                                                              static_cast<uint32_t>(this->ms32_Index),
                                                                              &c_Box, this->me_Type) == C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::UpdateShowValue(void)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->UpdateValue();
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]  ou32_DataElementIndex   Index of shown datapool element in widget
   \param[in]  os32_Value              Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->UpdateTransparency(ou32_DataElementIndex, os32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      tgl_assert(this->mpc_TableWidget != NULL);
      if (this->mpc_TableWidget != NULL)
      {
         this->mpc_TableWidget->UpdateStaticValues();
      }
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::EditElementProperties(void)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if ((this->mpc_TableWidget != NULL) &&
       (this->mq_EditContentModeEnabled == true))
   {
      const std::vector<uint32_t> c_Indices = this->mpc_TableWidget->GetUniqueAndValidSelectedRows();

      if (c_Indices.size() == 1)
      {
         const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

         if (pc_Dashboard != NULL)
         {
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32_t>(this->ms32_Index));
            tgl_assert(pc_Box != NULL);
            if ((pc_Box != NULL) && (pc_Box->c_DataPoolElementsConfig.size() > c_Indices[0]))
            {
               C_PuiSvDbNodeDataPoolListElementId c_ElementId;
               C_PuiSvDbDataElementScaling c_Scaling;
               C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig;
               QString c_DisplayName;
               QGraphicsView * const pc_View = this->scene()->views().at(0);
               const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
               C_SyvDaPeBase * pc_Dialog;
               c_ElementId = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_ElementId;
               c_Scaling = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_ElementScaling;
               c_FormatterConfig = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_DisplayFormatter;
               c_DisplayName = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_DisplayName;

               pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                             "Table", c_ElementId, c_Scaling, true, c_FormatterConfig, true,
                                             this->mq_DarkMode, false, false,
                                             c_DisplayName);

               //Resize
               c_New->SetSize(C_SyvDaPeBase::h_GetPopupSizeWithoutDesignAndPreview());

               if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
               {
                  C_PuiSvDbTable c_Box = *pc_Box;
                  C_PuiSvDbNodeDataElementConfig c_Tmp;

                  c_Tmp.c_ElementId = c_ElementId;
                  tgl_assert(c_ElementId == pc_Dialog->GetDataElementId());
                  c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
                  c_Tmp.c_DisplayFormatter = pc_Dialog->GetFormatterInformation();
                  c_Tmp.c_DisplayName = pc_Dialog->GetDisplayName();
                  c_Box.c_DataPoolElementsConfig[c_Indices[0]] = c_Tmp;

                  //Apply
                  this->RemoveDataPoolElement(c_ElementId);
                  this->RegisterDataPoolElement(c_Tmp.c_ElementId, c_Tmp.c_ElementScaling, c_Tmp.c_DisplayFormatter);

                  tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                                 this->mu32_DashboardIndex,
                                                                                 static_cast<uint32_t>(this->ms32_Index),
                                                                                 &c_Box, this->me_Type) == C_NO_ERR);
                  this->mpc_TableWidget->UpdateStaticValues();
               }
               Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
               if (c_New != NULL)
               {
                  c_New->HideOverlay();
                  c_New->deleteLater();
               }
            } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of edit mode

   \param[in]  oq_Active   Flag if edit mode is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::EditModeActiveChanged(const bool oq_Active)
{
   C_GiSvDaRectBaseGroup::EditModeActiveChanged(oq_Active);

   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      if (oq_Active == false)
      {
         this->mpc_TableWidget->SetSelectionAvailable(true, false);
      }
      else if (this->mq_EditContentModeEnabled == false)
      {
         this->mpc_TableWidget->SetSelectionAvailable(false, false);
      }
      else
      {
         // Nothing to do
      }

      this->mpc_TableWidget->SetCursorHandlingActive((this->mq_EditModeActive == false) ||
                                                     (this->mq_EditContentModeEnabled == true));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable edit content mode for widgets

   \retval false  nothing done
   \retval true   edit content mode enabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::EnableEditContent(void)
{
   const bool q_Return = C_GiSvDaRectBaseGroup::EnableEditContent();

   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      if (q_Return == true)
      {
         this->mpc_TableWidget->SetSelectionAvailable(true, true);
      }
      this->mpc_TableWidget->SetCursorHandlingActive((this->mq_EditModeActive == false) ||
                                                     (this->mq_EditContentModeEnabled == true));
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::DisableEditContent(void)
{
   C_GiSvDaRectBaseGroup::DisableEditContent();

   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->SetSelectionAvailable(false, false);
      // In case of a still visible tool tip, it will no disappear due to the not forwarded events and an own scene
      // independent handling, it must be hided manually
      this->mpc_TableWidget->HideToolTip();

      this->mpc_TableWidget->SetCursorHandlingActive((this->mq_EditModeActive == false) ||
                                                     (this->mq_EditContentModeEnabled == true));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates all relevant context menu entries for this item

   Context menu functions:
   - Add data element
   - Remove data element

   \param[in]  opc_ContextMenuManager  Pointer to context menu manager for registration of actions
   \param[in]  oq_Active               Flag if context menu entries have to be shown or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                             const bool oq_Active)
{
   if (this->mq_EditContentModeEnabled == false)
   {
      // Normal edit mode, use default dashboard context menu
      C_GiSvDaRectBaseGroup::ConfigureContextMenu(opc_ContextMenuManager, oq_Active);
   }
   else
   {
      // Deactivate the default dashboard scene actions.
      // In content edit mode only widget specific functions are relevant
      opc_ContextMenuManager->SetSpecificActionsAvailable(false, true);

      tgl_assert(this->mpc_TableWidget != NULL);
      if ((oq_Active == true) && (this->mpc_TableWidget != NULL))
      {
         const std::vector<uint32_t> c_SelectedRows = this->mpc_TableWidget->GetUniqueSelectedRows();
         // Initial registration of the context menu
         if (mpc_AddDataElement == NULL)
         {
            mpc_AddDataElement =
               opc_ContextMenuManager->RegisterActionWithKeyboardShortcut(C_GtGetText::h_GetText(
                                                                             "Add data element(s)"),
                                                                          static_cast<int32_t>(Qt::CTRL) +
                                                                          static_cast<int32_t>(Qt::Key_Plus));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_AddDataElement->setVisible(false);
            connect(mpc_AddDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_AddNewDataElement);
         }
         if (mpc_AddSeperator == NULL)
         {
            mpc_AddSeperator = opc_ContextMenuManager->RegisterSeperator();
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_AddSeperator->setVisible(false);
         }
         if (mpc_ConfigDataElement == NULL)
         {
            mpc_ConfigDataElement = opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText(
                                                                              "Edit Properties"));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_ConfigDataElement->setVisible(false);
            connect(mpc_ConfigDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::EditElementProperties);
         }
         if (mpc_ConfigSeperator == NULL)
         {
            mpc_ConfigSeperator = opc_ContextMenuManager->RegisterSeperator();
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_ConfigSeperator->setVisible(false);
         }
         if (mpc_MoveUpDataElement == NULL)
         {
            mpc_MoveUpDataElement =
               opc_ContextMenuManager->RegisterActionWithKeyboardShortcut(C_GtGetText::h_GetText(
                                                                             "Move up"),
                                                                          static_cast<int32_t>(Qt::CTRL) +
                                                                          static_cast<int32_t>(Qt::Key_Up));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_MoveUpDataElement->setVisible(false);
            connect(mpc_MoveUpDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementUp);
         }
         if (mpc_MoveDownDataElement == NULL)
         {
            mpc_MoveDownDataElement =
               opc_ContextMenuManager->RegisterActionWithKeyboardShortcut(C_GtGetText::h_GetText(
                                                                             "Move down"),
                                                                          static_cast<int32_t>(Qt::CTRL) +
                                                                          static_cast<int32_t>(Qt::Key_Down));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_MoveDownDataElement->setVisible(false);
            connect(mpc_MoveDownDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementDown);
         }
         if (mpc_MiscSeperator == NULL)
         {
            mpc_MiscSeperator = opc_ContextMenuManager->RegisterSeperator();
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_MiscSeperator->setVisible(false);
         }
         if (mpc_RemoveDataElement == NULL)
         {
            mpc_RemoveDataElement =
               opc_ContextMenuManager->RegisterActionWithKeyboardShortcut(C_GtGetText::h_GetText(
                                                                             "Delete"),
                                                                          static_cast<int32_t>(Qt::Key_Delete));
            // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
            this->mpc_RemoveDataElement->setVisible(false);
            connect(mpc_RemoveDataElement, &QAction::triggered,
                    this, &C_GiSvDaTableBase::m_RemoveDataElement);
         }

         // Connect the signals
         if ((mpc_AddDataElement != NULL) &&
             (this->GetWidgetDataPoolElementCount() < C_SyvDaItTaModel::hu32_MAX_ELEMENTS))
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddDataElement);
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddSeperator);
         }
         if (c_SelectedRows.size() > 0UL)
         {
            if (mpc_ConfigDataElement != NULL)
            {
               opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_ConfigDataElement);
               if (this->mpc_TableWidget->GetUniqueAndValidSelectedRows().size() == 1)
               {
                  this->mpc_ConfigDataElement->setEnabled(true);
               }
               else
               {
                  this->mpc_ConfigDataElement->setEnabled(false);
               }
               opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_ConfigSeperator);
            }
            if (this->mpc_MoveDownDataElement != NULL)
            {
               opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MoveDownDataElement);
            }
            if (this->mpc_MoveUpDataElement != NULL)
            {
               opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MoveUpDataElement);
            }
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MiscSeperator);
            if (this->mpc_RemoveDataElement != NULL)
            {
               opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_RemoveDataElement);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element

   This function is thread safe.

   \param[in]      ou32_WidgetDataPoolElementIndex    Index of shown datapool element in widget
   \param[out]     orc_Values                         Vector with result value(s)
   \param[in,out]  orc_DisplayValues                  Display values

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaTableBase::GetLastValueUnscaled(const uint32_t ou32_WidgetDataPoolElementIndex,
                                                std::vector<float64_t> & orc_Values,
                                                std::vector<QString> & orc_DisplayValues)
{
   uint32_t u32_InternalIndex;
   int32_t s32_Retval = this->m_MapDataElementIndexToInternalElementIndex(ou32_WidgetDataPoolElementIndex,
                                                                          u32_InternalIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->m_GetLastValue(ou32_WidgetDataPoolElementIndex, orc_DisplayValues, orc_Values);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table data item

   \return
   NULL Error
   Else Pointer to table data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbTable * C_GiSvDaTableBase::GetTableItem(void) const
{
   const C_PuiSvDbTable * pc_Retval = NULL;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if ((pc_Dashboard != NULL) && (this->GetIndex() >= 0))
      {
         pc_Retval = pc_Dashboard->GetTable(static_cast<uint32_t>(this->GetIndex()));
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new table content

   WARNING: Data element changes have to trigger RegisterDataElement

   \param[in]  orc_Content    New content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaTableBase::SetTableItem(const C_PuiSvDbTable & orc_Content) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32_t>(this->ms32_Index),
                                                                       &orc_Content, C_PuiSvDbDataElement::eTABLE);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check view is active

   \param[in]  orc_DataPoolElementId   Datapool element ID

   \return
   True  View active
   False View not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::GetViewActive(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const
{
   bool q_Retval = false;

   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      std::vector<uint8_t> c_NodeActiveFlags;
      const int32_t s32_FuncRetval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
         this->mu32_ViewIndex,
         c_NodeActiveFlags);

      if (s32_FuncRetval == C_NO_ERR)
      {
         q_Retval = static_cast<bool>(c_NodeActiveFlags[orc_DataPoolElementId.u32_NodeIndex]);
      }
      else
      {
         //Failure
         q_Retval = false;
      }
   }
   else
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         C_OscCanMessageIdentificationIndices c_MessageId;
         uint32_t u32_SignalIndex;
         if ((pc_View->GetOscPcData().GetConnected() == true) &&
             (C_PuiSdUtil::h_ConvertIndex(orc_DataPoolElementId, c_MessageId, u32_SignalIndex) == C_NO_ERR))
         {
            const C_OscNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_MessageId.u32_NodeIndex);
            if ((pc_Node != NULL) && (c_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
            {
               const C_OscNodeComInterfaceSettings & rc_Interface =
                  pc_Node->c_Properties.c_ComInterfaces[c_MessageId.u32_InterfaceIndex];
               if (rc_Interface.GetBusConnected() == true)
               {
                  if (rc_Interface.u32_BusIndex == pc_View->GetOscPcData().GetBusIndex())
                  {
                     //Active
                     q_Retval = true;
                  }
                  else
                  {
                     //Inactive
                     q_Retval = false;
                  }
               }
               else
               {
                  //Failure
                  q_Retval = false;
               }
            }
            else
            {
               //Failure
               q_Retval = false;
            }
         }
         else
         {
            //Failure
            q_Retval = false;
         }
      }
      else
      {
         //Failure
         q_Retval = false;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check node dashboard route is valid without errors

   \param[in]  orc_DataPoolElementId   Datapool element ID

   \return
   True  Node dashboard route is valid
   False Node dashboard route is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::GetViewDashboardRouteValid(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
const
{
   // In case of bus element irrelevant
   bool q_Return = true;

   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      bool q_Error;
      if (C_PuiSvHandler::h_GetInstance()->CheckViewNodeDashboardRoutingError(this->mu32_ViewIndex,
                                                                              orc_DataPoolElementId.u32_NodeIndex,
                                                                              q_Error) == C_NO_ERR)
      {
         q_Return = !q_Error;
      }
      else
      {
         q_Return = false;
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current view index

   \return
   Current view index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiSvDaTableBase::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check item error

   \param[in]   orc_DataPoolElementId     Datapool element ID
   \param[out]  orc_Content               Active error description
   \param[out]  orq_IsTransmissionError   Flag if transmission error occurred

   \return
   True  Error active
   False Error not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::CheckItemError(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                       QString & orc_Content, bool & orq_IsTransmissionError) const
{
   bool q_Retval = false;
   const QMap<C_PuiSvDbNodeDataPoolListElementId,
              QString>::const_iterator c_It = this->mc_CommmunicationErrors.find(orc_DataPoolElementId);

   if (c_It != this->mc_CommmunicationErrors.end())
   {
      q_Retval = true;
      orc_Content = c_It.value();
   }

   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL)
   {
      const QMap<C_OscNodeDataPoolListElementId, uint8_t>::const_iterator c_ItInvalidDlc =
         this->mc_InvalidDlcSignals.find(orc_DataPoolElementId);
      if (c_ItInvalidDlc != this->mc_InvalidDlcSignals.end())
      {
         q_Retval = true;
         orc_Content += static_cast<QString>(C_GtGetText::h_GetText("%1 had invalid DLC %2.")).
                        arg(C_PuiSdUtil::h_GetSignalNamespace(c_ItInvalidDlc.key())).
                        arg(QString::number(c_ItInvalidDlc.value()));
      }
      orq_IsTransmissionError = false;
   }
   else
   {
      orq_IsTransmissionError = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon

   Here: update table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_UpdateErrorIcon(void)
{
   //Don't call parent to not show any error icon
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->UpdateError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if warning icon is allowed

   \return
   True  Warning icon is allowed
   False Warning icon is not allowed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::m_AllowWarningIcon(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if element supports the edit content mode

   \return
   True  Element does have and support a widget specific edit content mode
   False Element does not have and support a widget specific edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::m_HasEditContentMode(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get common tool tip content if no other item takes precedence over the tool tip

   \return
   Common tool tip content if no other item takes precedence over the tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiSvDaTableBase::m_GetCommonToolTipContent(void) const
{
   //No common tool tip!
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_AddNewDataElement(void)
{
   //Trigger refresh of data because of fix for 73815
   this->UpdateData();
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      QGraphicsView * const pc_View = this->scene()->views().at(0);
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
      C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, true,
                                                                                    false, true, true, true, false);

      //Resize
      c_New->SetSize(C_SyvDaPeBase::h_GetPopupSizeWithoutDisplayFormatter());

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
         //Cursor
         QApplication::setOverrideCursor(Qt::WaitCursor);
         if (c_DataElements.size() > 0)
         {
            uint32_t u32_Counter;

            for (u32_Counter = 0U;
                 (u32_Counter < c_DataElements.size()) &&
                 (this->GetWidgetDataPoolElementCount() < C_SyvDaItTaModel::hu32_MAX_ELEMENTS); ++u32_Counter)
            {
               const C_PuiSvDbNodeDataPoolListElementId & rc_DataElement = c_DataElements[u32_Counter];
               if (rc_DataElement.GetIsValid() == true)
               {
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(rc_DataElement) == C_NO_ERR);
                  this->m_RegisterDataElementRail(rc_DataElement);
                  this->mpc_TableWidget->AddItem(rc_DataElement);
               }
            }
         }
         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
         //Trigger reload of data because of 73815
         this->LoadData();
         //Cursor
         QApplication::restoreOverrideCursor();
      }

      if (c_New != NULL)
      {
         pc_Dialog->SaveUserSettings();
         pc_Dialog->PrepareCleanUp();
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move selected items up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_MoveDataElementUp(void)
{
   if (this->mq_EditContentModeEnabled)
   {
      if (this->mpc_TableWidget != NULL)
      {
         this->mpc_TableWidget->MoveSelected(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move selected items down
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_MoveDataElementDown(void)
{
   if (this->mq_EditContentModeEnabled)
   {
      if (this->mpc_TableWidget != NULL)
      {
         this->mpc_TableWidget->MoveSelected(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_RemoveDataElement(void)
{
   if (this->mq_EditContentModeEnabled)
   {
      tgl_assert(this->mpc_TableWidget != NULL);
      if (this->mpc_TableWidget != NULL)
      {
         const C_PuiSvData * pc_View;

         std::vector<C_PuiSvDbNodeDataPoolListElementId> c_RemovedDataElements;
         //Remove data element(s)
         this->mpc_TableWidget->RemoveSelectedItems(c_RemovedDataElements);
         //Necessary before view is checked
         for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < c_RemovedDataElements.size(); ++u32_ItDataElement)
         {
            const C_PuiSvDbNodeDataPoolListElementId & rc_RemovedItem = c_RemovedDataElements[u32_ItDataElement];
            if (rc_RemovedItem.GetIsValid() == true)
            {
               this->RemoveDataPoolElement(rc_RemovedItem);
            }
         }
         //Remove read rail assignments as necessary
         pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            for (uint32_t u32_ItDataElement = 0; u32_ItDataElement < c_RemovedDataElements.size(); ++u32_ItDataElement)
            {
               const C_PuiSvDbNodeDataPoolListElementId & rc_RemovedItem = c_RemovedDataElements[u32_ItDataElement];
               if (rc_RemovedItem.GetIsValid() == true)
               {
                  if (pc_View->CheckReadUsage(rc_RemovedItem) == false)
                  {
                     //Allow error as the returned vector might not be unique (same id can occur multiple times)
                     C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                             rc_RemovedItem);
                  }
               }
            }
         }

         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register new data element rail assignment for new data element

   \param[in]  orc_DataPoolElementId   New data element ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_RegisterDataElementRail(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
const
{
   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvReadDataConfiguration c_Config;
         if ((((pc_Element->GetArray() == true) || (pc_Element->GetType() == C_OscNodeDataPoolContent::eUINT64)) ||
              (pc_Element->GetType() == C_OscNodeDataPoolContent::eSINT64)) ||
             (pc_Element->GetType() == C_OscNodeDataPoolContent::eFLOAT64))
         {
            c_Config.u8_RailIndex = 0;
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
         }
         else
         {
            c_Config.u8_RailIndex = 1;
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
         }
         c_Config.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
         tgl_assert(C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, orc_DataPoolElementId,
                                                                         c_Config) != C_RANGE);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map widget data element index to data element handler index

   \param[in]   ou32_DataElementIndex        Widget data element index
   \param[out]  oru32_InternalElementIndex   Data element handler index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaTableBase::m_MapDataElementIndexToInternalElementIndex(const uint32_t ou32_DataElementIndex,
                                                                       uint32_t & oru32_InternalElementIndex) const
{
   int32_t s32_Retval = C_RANGE;
   const C_PuiSvDbTable * const pc_Table = this->GetTableItem();

   if (pc_Table != NULL)
   {
      if (ou32_DataElementIndex < pc_Table->c_DataPoolElementsConfig.size())
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Table->c_DataPoolElementsConfig[ou32_DataElementIndex];
         s32_Retval = this->GetWidgetDataPoolElementIndex(rc_Config.c_ElementId, oru32_InternalElementIndex);
      }
   }
   return s32_Retval;
}
