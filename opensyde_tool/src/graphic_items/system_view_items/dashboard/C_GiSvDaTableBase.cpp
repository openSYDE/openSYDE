//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard table item (implementation)

   Class for system view dashboard table item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include <QGraphicsView>

#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdUtil.h"
#include "C_SyvDaPeBase.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_GiSvDaTableBase.h"
#include "C_SyvDaPeDataElementBrowse.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
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

   \param[in]     oru32_ViewIndex        Index of system view
   \param[in]     oru32_DashboardIndex   Index of dashboard in system view
   \param[in]     ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]     oru64_ID               Unique ID
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaTableBase::C_GiSvDaTableBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                     const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                     QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTABLE,
                         C_SyvDaItTaModel::hu32_MaxElements, oru64_ID, 50.0, 25.0, 100.0, 50.0, false, true,
                         opc_Parent),
   mpc_AddDataElement(NULL),
   mpc_AddSeperator(NULL),
   mpc_ConfigDataElement(NULL),
   mpc_RemoveDataElement(NULL),
   mpc_MoveUpDataElement(NULL),
   mpc_MoveDownDataElement(NULL),
   mpc_MiscSeperator(NULL)
{
   this->mpc_TableWidget = new C_SyvDaItTaView(this);
   this->mpc_Widget->SetWidget(this->mpc_TableWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaTableBase::~C_GiSvDaTableBase(void)
{
   //lint -e{1540} Either handled by Qt parent handling or not owned by this class in the first place
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaTableBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_TABLE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
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
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32>(this->ms32_Index));
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
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32>(this->ms32_Index));
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
                                                                              static_cast<uint32>(this->ms32_Index),
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
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in]     ou32_DataElementIndex     Index of shown datapool element in widget
   \param[in]     osn_Value                 Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->UpdateTransparence(ou32_DataElementIndex, osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in] oq_Active Flag if connection is active or not active now
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
/*! \brief   Information about the start or stop of edit mode

   \param[in]  oq_Active  Flag if edit mode is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::EditModeActiveChanged(const bool oq_Active)
{
   C_GiSvDaRectBaseGroup::EditModeActiveChanged(oq_Active);
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->SetSelectionAvailable(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::CallProperties(void)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      const std::vector<uint32> c_Indices = this->mpc_TableWidget->GetUniqueAndValidSelectedRows();

      if (c_Indices.size() == 1)
      {
         const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

         if (pc_Dashboard != NULL)
         {
            const C_PuiSvDbTable * const pc_Box = pc_Dashboard->GetTable(static_cast<uint32>(this->ms32_Index));
            tgl_assert(pc_Box != NULL);
            if ((pc_Box != NULL) && (pc_Box->c_DataPoolElementsConfig.size() > c_Indices[0]))
            {
               C_PuiSvDbNodeDataPoolListElementId c_ElementId;
               C_PuiSvDbDataElementScaling c_Scaling;
               QString c_DisplayName;
               QGraphicsView * const pc_View = this->scene()->views().at(0);
               QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
               C_SyvDaPeBase * pc_Dialog;
               c_ElementId = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_ElementId;
               c_Scaling = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_ElementScaling;
               c_DisplayName = pc_Box->c_DataPoolElementsConfig[c_Indices[0]].c_DisplayName;

               pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                             "Table", c_ElementId, c_Scaling, true, this->mq_DarkMode, false, false,
                                             c_DisplayName);

               //Resize
               c_New->SetSize(QSize(800, 500));

               if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
               {
                  C_PuiSvDbTable c_Box = *pc_Box;
                  C_PuiSvDbNodeDataElementConfig c_Tmp;

                  c_Tmp.c_ElementId = c_ElementId;
                  tgl_assert(c_ElementId == pc_Dialog->GetDataElementId());
                  c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
                  c_Tmp.c_DisplayName = pc_Dialog->GetDisplayName();
                  c_Box.c_DataPoolElementsConfig[c_Indices[0]] = c_Tmp;

                  //Apply
                  this->RemoveDataPoolElement(c_ElementId);
                  this->RegisterDataPoolElement(c_Tmp.c_ElementId, c_Tmp.c_ElementScaling);

                  tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                                 this->mu32_DashboardIndex,
                                                                                 static_cast<uint32>(this->ms32_Index),
                                                                                 &c_Box, this->me_Type) == C_NO_ERR);
                  this->mpc_TableWidget->UpdateStaticValues();
               }
               Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
               if (c_New != NULL)
               {
                  c_New->HideOverlay();
               }
               //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
            }
         }
      }
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates all relevant context menu entries for this item

   Context menu functions:
   - Add data element
   - Remove data element

   \param[in] opc_ContextMenuManager Pointer to context menu manager for registration of actions
   \param[in] oq_Active              Flag if context menu entries have to be shown or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                             const bool oq_Active)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if ((oq_Active == true) && (this->mpc_TableWidget != NULL))
   {
      const std::vector<uint32> c_SelectedRows = this->mpc_TableWidget->GetUniqueSelectedRows();
      // Initial registration of the context menu
      if (mpc_AddDataElement == NULL)
      {
         mpc_AddDataElement = opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Add data element(s)"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_AddDataElement->setVisible(false);
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
                                                                           "Selected data element properties"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_ConfigDataElement->setVisible(false);
      }
      if (mpc_RemoveDataElement == NULL)
      {
         mpc_RemoveDataElement =
            opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Remove selected data element"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_RemoveDataElement->setVisible(false);
      }
      if (mpc_MoveUpDataElement == NULL)
      {
         mpc_MoveUpDataElement =
            opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Move selected data element up"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_MoveUpDataElement->setVisible(false);
      }
      if (mpc_MoveDownDataElement == NULL)
      {
         mpc_MoveDownDataElement =
            opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Move selected data element down"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_MoveDownDataElement->setVisible(false);
      }
      if (mpc_MiscSeperator == NULL)
      {
         mpc_MiscSeperator = opc_ContextMenuManager->RegisterSeperator();
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_MiscSeperator->setVisible(false);
      }

      // Connect the signals
      if ((mpc_AddDataElement != NULL) &&
          (this->GetWidgetDataPoolElementCount() < C_SyvDaItTaModel::hu32_MaxElements))
      {
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddDataElement);
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddSeperator);
         connect(mpc_AddDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_AddNewDataElement);
      }
      if (c_SelectedRows.size() > 0UL)
      {
         if (mpc_ConfigDataElement != NULL)
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_ConfigDataElement);
            connect(mpc_ConfigDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::CallProperties);
            if (this->mpc_TableWidget->GetUniqueAndValidSelectedRows().size() == 1)
            {
               this->mpc_ConfigDataElement->setEnabled(true);
            }
            else
            {
               this->mpc_ConfigDataElement->setEnabled(false);
            }
         }
         if (this->mpc_MoveDownDataElement != NULL)
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MoveDownDataElement);
            connect(mpc_MoveDownDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementDown);
         }
         if (this->mpc_MoveUpDataElement != NULL)
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MoveUpDataElement);
            connect(mpc_MoveUpDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementUp);
         }
         if (this->mpc_RemoveDataElement != NULL)
         {
            opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_RemoveDataElement);
            connect(mpc_RemoveDataElement, &QAction::triggered,
                    this, &C_GiSvDaTableBase::m_RemoveDataElement);
         }
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MiscSeperator);
      }
   }
   else
   {
      // Disconnect the signals
      if (mpc_AddDataElement != NULL)
      {
         disconnect(mpc_AddDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_AddNewDataElement);
      }
      if (mpc_ConfigDataElement != NULL)
      {
         disconnect(mpc_ConfigDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::CallProperties);
      }
      if (this->mpc_MoveDownDataElement != NULL)
      {
         disconnect(mpc_MoveDownDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementDown);
      }
      if (this->mpc_MoveUpDataElement != NULL)
      {
         disconnect(mpc_MoveUpDataElement, &QAction::triggered, this, &C_GiSvDaTableBase::m_MoveDataElementUp);
      }
      if (mpc_RemoveDataElement != NULL)
      {
         disconnect(mpc_RemoveDataElement, &QAction::triggered,
                    this, &C_GiSvDaTableBase::m_RemoveDataElement);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex       Index of shown datapool element in widget
   \param[out]    orc_Values                            Vector with result value(s)

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiSvDaTableBase::GetLastValueUnscaled(const uint32 ou32_WidgetDataPoolElementIndex,
                                               std::vector<float64> & orc_Values)
{
   uint32 u32_InternalIndex;
   sint32 s32_Retval = this->m_MapDataElementIndexToInternalElementIndex(ou32_WidgetDataPoolElementIndex,
                                                                         u32_InternalIndex);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->m_GetLastValue(ou32_WidgetDataPoolElementIndex, orc_Values, NULL, false);
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
         pc_Retval = pc_Dashboard->GetTable(static_cast<uint32>(this->GetIndex()));
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new table content

   WARNING: Data element changes have to trigger RegisterDataElement

   \param[in] orc_Content New content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiSvDaTableBase::SetTableItem(const C_PuiSvDbTable & orc_Content) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->ms32_Index >= 0)
   {
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32>(this->ms32_Index),
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

   \param[in] orc_DataPoolElementId Datapool element ID

   \return
   True  View active
   False View not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::GetViewActive(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const
{
   bool q_Retval = false;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
      {
         const std::vector<uint8> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();
         if (orc_DataPoolElementId.u32_NodeIndex < rc_ActiveFlags.size())
         {
            q_Retval = static_cast<bool>(rc_ActiveFlags[orc_DataPoolElementId.u32_NodeIndex]);
         }
      }
      else
      {
         C_OSCCanMessageIdentificationIndices c_MessageID;
         uint32 u32_SignalIndex;
         if ((pc_View->GetPcData().GetConnected() == true) &&
             (C_PuiSdUtil::h_ConvertIndex(orc_DataPoolElementId, c_MessageID, u32_SignalIndex) == C_NO_ERR))
         {
            const C_OSCNode * const pc_Node =
               C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_MessageID.u32_NodeIndex);
            if ((pc_Node != NULL) && (c_MessageID.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
            {
               const C_OSCNodeComInterfaceSettings & rc_Interface =
                  pc_Node->c_Properties.c_ComInterfaces[c_MessageID.u32_InterfaceIndex];
               if (rc_Interface.q_IsBusConnected == true)
               {
                  if (rc_Interface.u32_BusIndex == pc_View->GetPcData().GetBusIndex())
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
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current view index

   \return
   Current view index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_GiSvDaTableBase::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check item error

   \param[in]  orc_DataPoolElementId   Datapool element ID
   \param[out] orc_Content             Active error description
   \param[out] orq_IsTransmissionError Flag if transmission error occurred

   \return
   True  Error active
   False Error not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaTableBase::CheckItemError(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                       QString & orc_Content, bool & orq_IsTransmissionError) const
{
   bool q_Retval = false;
   const QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId,
              QString>::const_iterator c_It = this->mc_CommmunicationErrors.find(orc_DataPoolElementId);

   if (c_It != this->mc_CommmunicationErrors.end())
   {
      q_Retval = true;
      orc_Content = c_It.value();
   }

   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL)
   {
      QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, stw_types::uint8>::const_iterator c_ItInvalidDlc =
         this->mc_InvalidDlcSignals.find(orc_DataPoolElementId);
      if (c_ItInvalidDlc != this->mc_InvalidDlcSignals.end())
      {
         q_Retval = true;
         orc_Content += QString(C_GtGetText::h_GetText("%1 had invalid DLC %2.")).arg(
            C_PuiSdHandler::h_GetInstance()->GetSignalNamespace(c_ItInvalidDlc.key())).arg(QString::number(
                                                                                              c_ItInvalidDlc.value()));
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
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      QGraphicsView * const pc_View = this->scene()->views().at(0);
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
      C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, true,
                                                                                    false, true, true, true, false);

      //Resize
      c_New->SetSize(QSize(800, 800));

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
         //Cursor
         QApplication::setOverrideCursor(Qt::WaitCursor);
         if (c_DataElements.size() > 0)
         {
            uint32 u32_Counter;

            for (u32_Counter = 0U;
                 (u32_Counter < c_DataElements.size()) &&
                 (this->GetWidgetDataPoolElementCount() < C_SyvDaItTaModel::hu32_MaxElements); ++u32_Counter)
            {
               const C_PuiSvDbNodeDataPoolListElementId & rc_DataElement = c_DataElements[u32_Counter];
               if (rc_DataElement.GetIsValid() == true)
               {
                  this->m_RegisterDataElementRail(rc_DataElement);
                  this->mpc_TableWidget->AddItem(rc_DataElement);
               }
            }
         }
         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
         //Cursor
         QApplication::restoreOverrideCursor();
      }

      if (c_New != NULL)
      {
         pc_Dialog->SaveUserSettings();
         c_New->HideOverlay();
      }
      //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move selected items up
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_MoveDataElementUp(void)
{
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->MoveSelected(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move selected items down
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_MoveDataElementDown(void)
{
   if (this->mpc_TableWidget != NULL)
   {
      this->mpc_TableWidget->MoveSelected(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_RemoveDataElement(void)
{
   tgl_assert(this->mpc_TableWidget != NULL);
   if (this->mpc_TableWidget != NULL)
   {
      const C_PuiSvData * pc_View;

      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_RemovedDataElements;
      //Remove data element(s)
      this->mpc_TableWidget->RemoveSelectedItems(c_RemovedDataElements);
      //Remove read rail assignments as necessary
      pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         for (uint32 u32_ItDataElement = 0; u32_ItDataElement < c_RemovedDataElements.size(); ++u32_ItDataElement)
         {
            const C_PuiSvDbNodeDataPoolListElementId & rc_RemovedItem = c_RemovedDataElements[u32_ItDataElement];
            if ((rc_RemovedItem.GetIsValid() == true) && (pc_View->CheckReadUsage(rc_RemovedItem) == false))
            {
               //Allow error as the returned vector might not be unique (same id can occur multiple times)
               C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                       rc_RemovedItem);
            }
         }
      }

      //Signal for error change
      Q_EMIT (this->SigDataElementsChanged());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register new data element rail assignment for new data element

   \param[in] orc_DataPoolElementId New data element ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTableBase::m_RegisterDataElementRail(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
const
{
   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvReadDataConfiguration c_Config;
         if ((((pc_Element->GetArray() == true) || (pc_Element->GetType() == C_OSCNodeDataPoolContent::eUINT64)) ||
              (pc_Element->GetType() == C_OSCNodeDataPoolContent::eSINT64)) ||
             (pc_Element->GetType() == C_OSCNodeDataPoolContent::eFLOAT64))
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

   \param[in]  ou32_DataElementIndex      Widget data element index
   \param[out] oru32_InternalElementIndex Data element handler index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiSvDaTableBase::m_MapDataElementIndexToInternalElementIndex(const uint32 ou32_DataElementIndex,
                                                                      uint32 & oru32_InternalElementIndex) const
{
   sint32 s32_Retval = C_RANGE;
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
