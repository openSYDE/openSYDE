//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all system view dashboard items which are rectangle based (implementation)

   Base class for all system view dashboard items which are rectangle based

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsView>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"

#include "C_Uti.hpp"
#include "C_SyvUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_PuiSvHandler.hpp"
#include "TglUtils.hpp"
#include "C_UtiStyleSheets.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64_t C_GiSvDaRectBaseGroup::mhf64_ACTION_POINT_OFFSET = 10.0;
const uint8_t C_GiSvDaRectBaseGroup::mhu8_START_GREY_TIMEOUT_PERCENTAGE = 20U;
const QString C_GiSvDaRectBaseGroup::mhc_ICON_READ = "://images/system_views/dashboards/icons/IconUpdateValueRead.svg";
const QString C_GiSvDaRectBaseGroup::mhc_ICON_READ_ABORT =
   "://images/system_views/dashboards/icons/IconUpdateValueReadCancel.svg";
const QString C_GiSvDaRectBaseGroup::mhc_ICON_READ_DISABLED =
   "://images/system_views/dashboards/icons/IconUpdateValueReadDisabled.svg";
const QString C_GiSvDaRectBaseGroup::mhc_ICON_WRITE =
   "://images/system_views/dashboards/icons/IconUpdateValueWrite.svg";
const QString C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_ABORT =
   "://images/system_views/dashboards/icons/IconUpdateValueWriteCancel.svg";
const QString C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_DISABLED =
   "://images/system_views/dashboards/icons/IconUpdateValueWriteDisabled.svg";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex            Index of system view
   \param[in]     oru32_DashboardIndex       Index of dashboard in system view
   \param[in]     ors32_DataIndex            Index of data element in dashboard in system view
   \param[in]     ore_Type                   Type for data storage
   \param[in]     ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in]     oru64_Id                   Unique ID
   \param[in]     of64_MinWidth              Minimum width of widget
   \param[in]     of64_MinHeight             Minimum height of widget
   \param[in]     of64_InitWidth             Initial width of widget
   \param[in]     of64_InitHeight            Initial height of widget
   \param[in]     oq_KeepAspectRatio         Flag if the rectangle should always keep its initial aspect ratio
   \param[in]     oq_ReadItem                Flag if item is a read only item and needs the timeout mechanism
   \param[in,out] opc_Parent                 Optional pointer to parent
   \param[in]     orc_PosOffset              Optional offset for the position (values must be in 0.0 <= x < 1.0)
                                             Negative value deactivates the function
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaRectBaseGroup::C_GiSvDaRectBaseGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                             const int32_t & ors32_DataIndex, const E_Type & ore_Type,
                                             const uint32_t ou32_MaximumDataElements, const uint64_t & oru64_Id,
                                             const float64_t of64_MinWidth, const float64_t of64_MinHeight,
                                             const float64_t of64_InitWidth, const float64_t of64_InitHeight,
                                             const bool oq_KeepAspectRatio, const bool oq_ReadItem,
                                             QGraphicsItem * const opc_Parent, const QPointF & orc_PosOffset) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_Id, of64_MinWidth, of64_MinHeight, mhf64_ACTION_POINT_OFFSET, oq_KeepAspectRatio,
                       opc_Parent, orc_PosOffset),
   C_PuiSvDbDataElementHandler(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, ore_Type,
                               ou32_MaximumDataElements, oq_ReadItem),
   me_Style(C_PuiSvDbWidgetBase::eOPENSYDE),
   me_WriteMode(C_PuiSvDbWidgetBase::eWM_MANUAL),
   mq_DarkMode(false),
   mf64_WriteValue(0.0),
   mq_InitialStyleCall(true),
   mq_EditModeActive(false),
   mq_EditContentModeEnabled(false),
   ms32_IconSize(24),
   mc_CurrentSize(of64_InitWidth, of64_InitHeight),
   mq_ProxyWidgetInteractionActive(false),
   // No valid value received yet -> Timeout as start value
   mc_LastTransparencyValue(ms32_TRANSPARENCY_START),
   mq_ConnectionActive(false),
   mq_ShowButton(false),
   mq_AbortTriggered(false),
   mu32_NextManualActionIndex(0)
{
   this->mpc_ProxyWidget = new C_GiWiProxyBase(of64_InitWidth, of64_InitHeight, this);
   mpc_Widget = new C_SyvDaItDashboardBaseWidget();
   this->mpc_Widget->resize(static_cast<int32_t>(of64_InitWidth), static_cast<int32_t>(of64_InitHeight));
   this->mpc_ProxyWidget->setWidget(this->mpc_Widget);
   this->addToGroup(this->mpc_ProxyWidget);
   this->m_SetBiggestItem(*this->mpc_ProxyWidget);

   this->m_InitConflictIcon();
   this->m_InitButton();

   this->setFlag(ItemUsesExtendedStyleOption);

   //Allow hover events for tool tip hide
   this->setAcceptHoverEvents(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ProxyWidget and mpc_ConflictIcon and the Qt memory
// management
C_GiSvDaRectBaseGroup::~C_GiSvDaRectBaseGroup(void)
{
   this->mpc_ProxyWidget->setWidget(NULL);
   delete mpc_Widget;
   this->mpc_Widget = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the cursor position is relevant for the proxy widget

   \param[in]     orc_ScenePos     Scene position of cursor

   \return
   true     Cursor is on a relevant position
   false    Cursor is not on a relevant position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos) const
{
   return this->mpc_ProxyWidget->contains(this->mpc_ProxyWidget->mapFromScene(orc_ScenePos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the necessary cursor type when hovered dependent of the cursor position

   Dependent of the resize action points

   \param[in]       orc_ScenePos     Scene position of cursor
   \param[out]      ore_Cursor       Detected necessary cursor

   \retval   true   Override cursor is necessary
   \retval   false  Override cursor is not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::IsOverrideCursorNecessary(const QPointF & orc_ScenePos, Qt::CursorShape & ore_Cursor) const
{
   bool q_Return = false;

   if ((this->mq_EditModeActive == true) &&
       (this->mq_EditContentModeEnabled == false))
   {
      q_Return = this->IsResizeCursorNecessary(orc_ScenePos, ore_Cursor);

      if (q_Return == false)
      {
         q_Return = true;
         ore_Cursor = Qt::SizeAllCursor;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style,
                                            const bool oq_DarkMode)
{
   if (((this->mq_InitialStyleCall == true) || (this->me_Style != oe_Style)) || (this->mq_DarkMode != oq_DarkMode))
   {
      this->mq_InitialStyleCall = false;
      this->me_Style = oe_Style;
      this->mq_DarkMode = oq_DarkMode;

      if (this->mpc_Widget != NULL)
      {
         switch (oe_Style)
         {
         case C_PuiSvDbWidgetBase::eOPENSYDE:
            if (oq_DarkMode == true)
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style", "OPENSYDE_DARK");
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style",
                                                                            "OPENSYDE_BRIGHT");
            }
            break;
         case C_PuiSvDbWidgetBase::eOPENSYDE_2:
            if (oq_DarkMode == true)
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style",
                                                                            "OPENSYDE_2_DARK");
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style",
                                                                            "OPENSYDE_2_BRIGHT");
            }
            break;
         case C_PuiSvDbWidgetBase::eFLAT:
            if (oq_DarkMode == true)
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style", "FLAT_DARK");
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style", "FLAT_BRIGHT");
            }
            break;
         case C_PuiSvDbWidgetBase::eSKEUOMORPH:
            if (oq_DarkMode == true)
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style",
                                                                            "SKEUOMORPH_DARK");
            }
            else
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Style",
                                                                            "SKEUOMORPH_BRIGHT");
            }
            break;
         default:
            //Use default Qt style
            break;
         }
         this->ReInitializeSize();
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::SetDefaultCursor(const QCursor & orc_Value)
{
   C_GiBiRectBaseGroup::SetDefaultCursor(orc_Value);
   // Dashboard specific: In case of changing into edit mode or from edit content mode to normal edit mode
   // all visible proxy widgets would not update the new set cursor. By setting explicit the cursor to the proxy widget
   // this problem is fixed
   this->mpc_ProxyWidget->setCursor(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load basic widget data

   \param[in] orc_Data Basic widget data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::LoadSvBasicData(const C_PuiSvDbWidgetBase & orc_Data)
{
   this->me_Style = orc_Data.e_DisplayStyle;
   this->me_WriteMode = orc_Data.e_ElementWriteMode;

   this->LoadBasicData(orc_Data);
   this->ApplySizeChange(orc_Data.c_UiPosition, QSizeF(orc_Data.f64_Width, orc_Data.f64_Height));
   this->ClearDataPoolElements();
   for (uint32_t u32_It = 0; u32_It < orc_Data.c_DataPoolElementsConfig.size(); ++u32_It)
   {
      if (orc_Data.c_DataPoolElementsConfig[u32_It].c_ElementId.GetIsValid() == true)
      {
         this->RegisterDataPoolElement(orc_Data.c_DataPoolElementsConfig[u32_It].c_ElementId,
                                       orc_Data.c_DataPoolElementsConfig[u32_It].c_ElementScaling,
                                       orc_Data.c_DataPoolElementsConfig[u32_It].c_DisplayFormatter);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update basic widget data

   \param[in,out]  orc_Data               Basic widget data
   \param[in]      oq_SkipDataElements    Flag for skipping data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::UpdateSvBasicData(C_PuiSvDbWidgetBase & orc_Data, const bool oq_SkipDataElements) const
{
   orc_Data.e_DisplayStyle = this->me_Style;
   orc_Data.e_ElementWriteMode = this->me_WriteMode;

   this->UpdateBasicData(orc_Data);
   if (oq_SkipDataElements == false)
   {
      const uint32_t u32_Count = GetWidgetDataPoolElementCount();

      orc_Data.c_DataPoolElementsConfig.clear();
      orc_Data.c_DataPoolElementsConfig.reserve(u32_Count);
      for (uint32_t u32_It = 0; u32_It < u32_Count; ++u32_It)
      {
         C_PuiSvDbNodeDataElementConfig c_Tmp;

         tgl_assert(GetDataPoolElementIndex(u32_It, c_Tmp.c_ElementId) == C_NO_ERR);
         tgl_assert(GetDataPoolElementScaling(u32_It, c_Tmp.c_ElementScaling) == C_NO_ERR);
         tgl_assert(GetDataPoolElementFormatter(u32_It, c_Tmp.c_DisplayFormatter) == C_NO_ERR);
         orc_Data.c_DataPoolElementsConfig.push_back(c_Tmp);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register cyclic transmission error for specific data element

   \param[in] orc_WidgetDataPoolElementId Affected data element
   \param[in] ou8_ErrorCode               Registered error code
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::RegisterDataPoolElementCyclicError(
   const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8_t ou8_ErrorCode)
{
   if (this->mq_ReadItem == true)
   {
      QString c_Info;
      QString c_AdditionalInfo;

      //Clean up any left over items in buffer
      this->UpdateShowValue();

      if (ou8_ErrorCode == 0x22)
      {
         c_AdditionalInfo = C_GtGetText::h_GetText("Access to element value failed (e.g. blocked by application)");
      }
      else
      {
         c_AdditionalInfo =
            static_cast<QString>(C_GtGetText::h_GetText("Unknown NRC: 0x%1")).arg(QString::number(ou8_ErrorCode, 16));
      }
      c_Info =
         static_cast<QString>(C_GtGetText::h_GetText("Cyclic service failed with error: %1")).arg(c_AdditionalInfo);
      this->mc_CommmunicationErrors.remove(orc_WidgetDataPoolElementId);
      this->mc_CommmunicationErrors.insert(orc_WidgetDataPoolElementId, c_Info);
      m_UpdateErrorIcon();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element

   If the dashboard element is a reading element the derived class must reimplement this function and handle
   the incoming values.
   Base class implementation checks for timeout only and adapts the styling of the items.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::UpdateShowValue(void)
{
   if (this->mq_ReadItem == true)
   {
      //Might happen in cases where transparency is not necessary
      if (this->GetWidgetDataPoolElementCount() == this->mc_LastTransparencyValue.size())
      {
         uint8_t u8_Percentage;
         int32_t s32_Transparency;
         //lint -e{778}  Kept in case the transparency engine is reactivated in the future
         const int32_t s32_DIFF = ms32_TRANSPARENCY_END - ms32_TRANSPARENCY_START;
         if (this->GetWidgetDataPoolElementCount() == 1)
         {
            if (this->m_GetTimoutPercentage100(0, u8_Percentage) != C_NO_ERR)
            {
               u8_Percentage = 100U;
            }

            if (u8_Percentage > 100U)
            {
               u8_Percentage = 100U;
            }

            // 0 Percentage equals value received equals no transparency -> 255
            // Tolerance percentage (mu32_DASHBOARD_TIMEOUT_TOLERANCE) till first expected receive -> 255
            // 100 Percentage equals timeout reached equals half transparency -> 127
            s32_Transparency = ms32_TRANSPARENCY_END;
            //lint -e{845}  Kept in case the transparency engine is reactivated in the future
            s32_Transparency -= ((static_cast<int32_t>(u8_Percentage) * s32_DIFF) / 100);

            if (s32_Transparency != this->mc_LastTransparencyValue[0])
            {
               C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Transparency",
                                                                            QString::number(s32_Transparency));

               // Update the transparency of not stylesheet items
               this->UpdateTransparency(0, s32_Transparency);

               // Font changed, trigger update
               this->mpc_ProxyWidget->update();

               this->mc_LastTransparencyValue[0] = s32_Transparency;
            }
         }
         else
         {
            for (uint32_t u32_It = 0; u32_It < this->GetWidgetDataPoolElementCount(); ++u32_It)
            {
               if (this->m_GetTimoutPercentage100(u32_It, u8_Percentage) != C_NO_ERR)
               {
                  u8_Percentage = 100U;
               }

               if (u8_Percentage > 100U)
               {
                  u8_Percentage = 100U;
               }

               // 0 Percentage equals value received equals no transparency -> 255
               // Tolerance percentage (mu32_DASHBOARD_TIMEOUT_TOLERANCE) till first expected receive -> 255
               // 100 Percentage equals timeout reached equals half transparency -> 127
               s32_Transparency = ms32_TRANSPARENCY_END;
               //lint -e{845}  Kept in case the transparency engine is reactivated in the future
               s32_Transparency -= ((static_cast<int32_t>(u8_Percentage) * s32_DIFF) / 100);

               if (s32_Transparency != this->mc_LastTransparencyValue[u32_It])
               {
                  // Update the transparency of not stylesheet items
                  this->UpdateTransparency(u32_It, s32_Transparency);

                  this->mc_LastTransparencyValue[u32_It] = s32_Transparency;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   Base class implementation does nothing. If the dashboard element has drawing elements which
   can not be adapted by stylesheets the derived class must reimplement this function.

   \param[in]     ou32_DataElementIndex     Index of shown datapool element in widget
   \param[in]     os32_Value                Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiSvDaRectBaseGroup::UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value)
{
   // Nothing to do in the base class implementation
   Q_UNUSED(ou32_DataElementIndex)
   Q_UNUSED(os32_Value)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now

   Base class implementation resets the timeout coloring.
   If the dashboard element wants to know when the
   connections starts the derived class must reimplement this function.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::ConnectionActiveChanged(const bool oq_Active)
{
   this->mq_ConnectionActive = oq_Active;

   if (oq_Active == true)
   {
      if (this->mq_ReadItem == true)
      {
         // Reset the timeout coloring
         C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this->mpc_Widget, "Transparency",
                                                                      QString::number(ms32_TRANSPARENCY_START));

         // Update the transparency of not stylesheet items
         for (uint32_t u32_It = 0; u32_It < this->GetWidgetDataPoolElementCount(); ++u32_It)
         {
            this->UpdateTransparency(u32_It, ms32_TRANSPARENCY_START);
         }

         // Font changed, trigger update
         this->mpc_ProxyWidget->update();

         this->mc_LastTransparencyValue.clear();
         this->mc_LastTransparencyValue.resize(this->GetWidgetDataPoolElementCount(), -1);

         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_READ);
      }
      else
      {
         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_WRITE);
      }

      this->mpc_ButtonGroup->setVisible(this->mq_ShowButton);

      // The configuration of the rail updaterates may have changed. Update it.
      this->m_UpdateDataPoolElementTimeoutAndValidFlag();
   }
   else
   {
      this->mpc_ButtonGroup->setVisible(this->mq_ShowButton);

      if (this->mq_ShowButton == true)
      {
         if (this->mq_ReadItem == true)
         {
            this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_READ_DISABLED);
         }
         else
         {
            this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_DISABLED);
         }
      }
      //Always disable error
      if (this->mpc_ConflictIcon != NULL)
      {
         this->mpc_ConflictIcon->setVisible(false);
         //Clear error
         this->mc_CommmunicationErrors.clear();
         this->mc_InvalidDlcSignals.clear();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of edit mode

   If the dashboard element has edit mode specific functionality,
   the derived class must reimplement this function.

   \param[in]  oq_Active  Flag if edit mode is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::EditModeActiveChanged(const bool oq_Active)
{
   this->mq_EditModeActive = oq_Active;

   if (oq_Active == false)
   {
      if (this->mq_EditContentModeEnabled == true)
      {
         // In case of a still active content edit mode and the deactivation of the edit mode, deactivate it
         this->DisableEditContent();
      }
      // Switching the cursor back to ArrowCursor, DisableEditContent would change it to SizeAllCursor
      this->SetDefaultCursor(Qt::ArrowCursor);
   }
   else
   {
      this->SetDefaultCursor(Qt::SizeAllCursor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the current set value of the element

   Unscales the actual value, sets the value in the datapool and sends the signal SigDataPoolWrite for writing
   the element to the server.
   The derived class must set the actual value into mf64_WriteValue before calling
   the base class implementation.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::SendCurrentValue(void)
{
   if ((this->mu32_NextManualActionIndex == 0) && (this->mpc_RefreshIcon != NULL))
   {
      m_ManualOperationStarted();
   }
   if ((this->GetWidgetDataPoolElementCount() >  this->mu32_NextManualActionIndex) &&
       (this->mq_ReadItem == false))
   {
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      C_PuiSvDbDataElementScaling c_Scaling;
      int32_t s32_Return;

      s32_Return = this->GetDataPoolElementIndex(this->mu32_NextManualActionIndex, c_ElementId);
      tgl_assert(s32_Return == C_NO_ERR);
      s32_Return = this->GetDataPoolElementScaling(this->mu32_NextManualActionIndex, c_Scaling);
      tgl_assert(s32_Return == C_NO_ERR);

      if ((c_ElementId.GetIsValid() == true) &&
          (this->m_CheckNodeActive(c_ElementId.u32_NodeIndex) == true) &&
          (this->m_CheckNodeHasAnyRequiredValidDashboardRouting(c_ElementId.u32_NodeIndex) == true))
      {
         // Update the datapool
         C_OscNodeDataPoolListElement c_OscElement;
         C_PuiSdNodeDataPoolListElement c_UiElement;

         s32_Return = C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(c_ElementId.u32_NodeIndex,
                                                                              c_ElementId.u32_DataPoolIndex,
                                                                              c_ElementId.u32_ListIndex,
                                                                              c_ElementId.u32_ElementIndex,
                                                                              c_OscElement,
                                                                              c_UiElement);

         tgl_assert(s32_Return == C_NO_ERR);
         if (s32_Return == C_NO_ERR)
         {
            // Update the value
            C_SdNdeDpContentUtil::h_SetScaledValueInContent(this->mf64_WriteValue, c_OscElement.c_Value,
                                                            c_Scaling.f64_Factor, c_Scaling.f64_Offset);
            //Compensate floating point precision
            if ((c_OscElement.c_Value <= c_OscElement.c_MaxValue) == false)
            {
               c_OscElement.c_Value = c_OscElement.c_MaxValue;
            }
            if ((c_OscElement.c_Value >= c_OscElement.c_MinValue) == false)
            {
               c_OscElement.c_Value = c_OscElement.c_MinValue;
            }
            //Apply
            s32_Return = C_PuiSdHandler::h_GetInstance()->SetDataPoolListElement(c_ElementId.u32_NodeIndex,
                                                                                 c_ElementId.u32_DataPoolIndex,
                                                                                 c_ElementId.u32_ListIndex,
                                                                                 c_ElementId.u32_ElementIndex,
                                                                                 c_OscElement,
                                                                                 c_UiElement);
            tgl_assert(s32_Return == C_NO_ERR);

            ++this->mu32_NextManualActionIndex;
            Q_EMIT this->SigDataPoolWrite(c_ElementId.u32_NodeIndex,
                                          static_cast<uint8_t>(c_ElementId.u32_DataPoolIndex),
                                          static_cast<uint16_t>(c_ElementId.u32_ListIndex),
                                          static_cast<uint16_t>(c_ElementId.u32_ElementIndex));
         }
         else
         {
            this->m_ManualOperationFinished();
         }
      }
      else
      {
         this->m_ManualOperationFinished();
      }
   }
   else
   {
      this->m_ManualOperationFinished();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   Base class implementation does nothing. If there are configurable properties for the dashboard element
   the derived class must reimplement this function.

   \retval false  nothing done
   \retval true   configurable properties called
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::CallProperties(void)
{
   // Nothing to do in the base class implementation
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable edit content mode for widgets

   If the element supports edit content mode, the mode will be enabled

   \retval false  nothing done
   \retval true   edit content mode enabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::EnableEditContent(void)
{
   bool q_Return = false;

   // Only available in the dashboard edit mode
   if (this->mq_EditModeActive == true)
   {
      q_Return = this->m_HasEditContentMode();

      if (q_Return == true)
      {
         this->mq_EditContentModeEnabled = true;
         // Deactivate moving functionality in this mode
         this->setFlag(QGraphicsItem::ItemIsMovable, false);
         this->SetDefaultCursor(Qt::ArrowCursor);

         this->update();
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable edit content mode for widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::DisableEditContent(void)
{
   if (this->mq_EditContentModeEnabled == true)
   {
      // Activate moving only here again due to no handling of moving functionality in the selection handling
      // unlike SetResizing
      this->setFlag(QGraphicsItem::ItemIsMovable, true);
      // In case of an override cursor (for example column resize of the table), restore any override cursor due to
      // missing event handling of the proxy widget when the edit content mode is disabled
      QApplication::restoreOverrideCursor();
      this->SetDefaultCursor(Qt::SizeAllCursor);

      this->mq_EditContentModeEnabled = false;
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the edit content mode

   \retval   true    Mode is enabled
   \retval   false   Mode is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::IsEditContentEnabled(void) const
{
   return this->mq_EditContentModeEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_Nrc     Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc)
{
   if (this->mu32_NextManualActionIndex > 0)
   {
      QGraphicsView * const pc_View = this->scene()->views().at(0);
      C_OgeWiCustomMessage c_Message(pc_View, C_OgeWiCustomMessage::E_Type::eERROR);
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      const int32_t s32_Return = this->GetDataPoolElementIndex(this->mu32_NextManualActionIndex - 1UL, c_ElementId);
      //Response error handling
      if (os32_Result != C_NO_ERR)
      {
         QString c_Description;
         QString c_Details;
         int32_t s32_MaxHeight = 0;

         this->m_GetErrorDescriptionForManualOperation(os32_Result, ou8_Nrc, c_Description, c_Details, s32_MaxHeight);

         osc_write_log_info("Manual operation",
                            static_cast<QString>("The C_SyvComDataDealer function ended with error code \"%1\"").arg(
                               C_Uti::h_StwError(os32_Result)).toStdString().c_str());
         if (s32_Return == C_NO_ERR)
         {
            //Change icon
            this->mc_CommmunicationErrors.remove(c_ElementId);
            this->mc_CommmunicationErrors.insert(c_ElementId, c_Description);
            this->m_UpdateErrorIcon();
         }
         else
         {
            if (s32_MaxHeight != 0)
            {
               // Change only if a concrete value was set
               c_Message.SetCustomMinHeight(180, s32_MaxHeight);
            }
            c_Message.SetHeading(C_GtGetText::h_GetText("Transmission failure"));
            c_Message.SetDescription(c_Description);
            c_Message.SetDetails(c_Details);
            c_Message.Execute();
         }
      }
      else
      {
         //Clear error if successful
         if (s32_Return == C_NO_ERR)
         {
            //Change icon
            this->mc_CommmunicationErrors.remove(c_ElementId);
            this->m_UpdateErrorIcon();
         }
      }
      //Continue after error
      //Don't continue if user abort
      if (this->mq_AbortTriggered)
      {
         this->mq_AbortTriggered = false;
         this->m_ManualOperationFinished();
      }
      else
      {
         if (this->mq_ReadItem == true)
         {
            this->m_ManualRead();
         }
         else
         {
            SendCurrentValue();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLC error for specified element id

   \param[in] orc_ElementId Element ID
   \param[in] ou8_Dlc       DLC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::SetErrorForInvalidDlc(const C_OscNodeDataPoolListElementId & orc_ElementId,
                                                  const uint8_t ou8_Dlc)
{
   this->mc_InvalidDlcSignals.insert(orc_ElementId, ou8_Dlc);
   m_UpdateErrorIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in] oq_Active Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiSvDaRectBaseGroup::SetDrawingActive(const bool oq_Active)
{
   Q_UNUSED(oq_Active)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                  QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   if (this->isSelected() == true)
   {
      //Increase selection rectangle size for system view items
      const QRectF c_Rect = this->GetVisibleBoundingRect();
      QPen c_Pen(static_cast<QBrush>(mc_STYLE_GUIDE_COLOR_21), 1.0);

      if (this->mq_EditContentModeEnabled == true)
      {
         c_Pen.setStyle(Qt::DashLine);
      }

      opc_Painter->setRenderHint(QPainter::HighQualityAntialiasing);
      opc_Painter->setClipRect(opc_Option->exposedRect);
      opc_Painter->setBrush(Qt::NoBrush);
      opc_Painter->setPen(c_Pen);
      opc_Painter->drawRect(c_Rect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates all relevant context menu entries for this item

   Base class implementation does nothing. If there are context menu actions for the dashboard element
   the derived class must reimplement this function.

   \param[in]     opc_ContextMenuManager  Pointer to context menu manager for registration of actions
   \param[in]     oq_Active               Flag if context menu entries have to be shown or not
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiSvDaRectBaseGroup::ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                                 const bool oq_Active)
{
   Q_UNUSED(oq_Active)

   // Activate the "default specific" actions again.
   // Can be deactivated by derived classes which do not want any of the default functions
   opc_ContextMenuManager->SetSpecificActionsAvailable(true, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get display style type

   \return
   Current display style type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbWidgetBase::E_Style C_GiSvDaRectBaseGroup::GetDisplayStyleType(void) const
{
   C_PuiSvDbWidgetBase::E_Style e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      tgl_assert((pc_Dashboard != NULL) && (this->ms32_Index >= 0));
      if ((pc_Dashboard != NULL) && (this->ms32_Index >= 0))
      {
         const C_PuiSvDbWidgetBase * const pc_WidgetBase =
            pc_Dashboard->GetWidgetBase(this->GetWidgetType(), static_cast<uint32_t>(this->ms32_Index));
         tgl_assert(pc_WidgetBase != NULL);
         if (pc_WidgetBase != NULL)
         {
            e_Retval = pc_WidgetBase->e_DisplayStyle;
         }
      }
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate hint for current widget
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_GiSvDaRectBaseGroup::GenerateHint(void)
{
   //Hint always up to date
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiRectBaseGroup::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the pointer to the associated dashboard data class

   \return
   - Pointer to dashboard if valid
   - NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDashboard * C_GiSvDaRectBaseGroup::m_GetSvDashboard(void) const
{
   const C_PuiSvDashboard * pc_Dashboard = NULL;

   tgl_assert(this->ms32_Index >= 0);
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      tgl_assert(pc_View != NULL);
      if (pc_View != NULL)
      {
         pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         tgl_assert(pc_Dashboard != NULL);
      }
   }

   return pc_Dashboard;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element as formatted and scaled string

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[out]    orc_ScaledValue                 String with result string
   \param[out]    opf64_UnscaledValueAsFloat      Optional float with unscaled value

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaRectBaseGroup::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, QString & orc_ScaledValue,
                                              float64_t * const opf64_UnscaledValueAsFloat)
{
   C_PuiSvDbNodeDataPoolListElementId c_Id;
   const int32_t s32_Retval = C_PuiSvDbDataElementHandler::m_GetLastValue(ou32_WidgetDataPoolElementIndex,
                                                                          orc_ScaledValue,
                                                                          opf64_UnscaledValueAsFloat);

   //Remove error on valid new value
   if ((s32_Retval == C_NO_ERR) && (this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CommmunicationErrors.remove(c_Id);
      this->mc_InvalidDlcSignals.remove(c_Id);
      m_UpdateErrorIcon();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex     Index of shown datapool element in widget
   \param[out]    orc_ScaledValues                   Vector with result scaled and formatted value(s)
   \param[out]    orc_UnscaledValues                 Vector with unscaled float values

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaRectBaseGroup::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                              std::vector<QString> & orc_ScaledValues,
                                              std::vector<float64_t> & orc_UnscaledValues)
{
   C_PuiSvDbNodeDataPoolListElementId c_Id;
   const int32_t s32_Retval = C_PuiSvDbDataElementHandler::m_GetLastValue(ou32_WidgetDataPoolElementIndex,
                                                                          orc_ScaledValues,
                                                                          orc_UnscaledValues);

   //Remove error on valid new value
   if ((s32_Retval == C_NO_ERR) && (this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CommmunicationErrors.remove(c_Id);
      this->mc_InvalidDlcSignals.remove(c_Id);
      m_UpdateErrorIcon();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the newest registered value of a specific datapool element. It can be an array too.

   The vector will be cleared first.

   If it is an array, the size of the vector is > 1
   If it is no array, the size of the vector is 1
   If an error occurred, the size of the vector is 0

   This function is thread safe.

   \param[in]     ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[out]    orc_Values                      Vector with result value(s)
   \param[in]     oq_UseScaling                   Optional flag if return value should include internally stored scaling

   \return
   C_NO_ERR    Value read
   C_RANGE     Index invalid
   C_NOACT     No value received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaRectBaseGroup::m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                              std::vector<float64_t> & orc_Values, const bool oq_UseScaling)
{
   C_PuiSvDbNodeDataPoolListElementId c_Id;
   const int32_t s32_Retval = C_PuiSvDbDataElementHandler::m_GetLastValue(ou32_WidgetDataPoolElementIndex, orc_Values,
                                                                          oq_UseScaling);

   //Remove error on valid new value
   if ((s32_Retval == C_NO_ERR) && (this->GetDataPoolElementIndex(ou32_WidgetDataPoolElementIndex, c_Id) == C_NO_ERR))
   {
      this->mc_CommmunicationErrors.remove(c_Id);
      this->mc_InvalidDlcSignals.remove(c_Id);
      m_UpdateErrorIcon();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle resize update for children

   \param[in] of64_DiffWidth  Width
   \param[in] of64_DiffHeight Height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight)
{
   this->mc_CurrentSize = QSizeF(this->mc_CurrentSize.width() + of64_DiffWidth,
                                 this->mc_CurrentSize.height() + of64_DiffHeight);

   this->mpc_ProxyWidget->resize(this->mc_CurrentSize);

   //Check if resize happened
   if (this->mc_CurrentSize != this->mpc_ProxyWidget->size())
   {
      //Force use of new size
      this->mpc_ProxyWidget->setMinimumSize(this->mc_CurrentSize);
      this->mpc_ProxyWidget->setMaximumSize(this->mc_CurrentSize);
      m_ForceWidgetResize(this->mc_CurrentSize);
   }

   if (this->mpc_ConflictIcon != NULL)
   {
      this->mpc_ConflictIcon->moveBy(of64_DiffWidth, 0.0);
      this->mpc_ConflictIcon->update();
   }
   if (this->mpc_WarningIcon != NULL)
   {
      this->mpc_WarningIcon->moveBy(of64_DiffWidth, 0.0);
      this->mpc_WarningIcon->update();
   }
   if (this->mpc_ButtonGroup != NULL)
   {
      this->mpc_ButtonGroup->moveBy(of64_DiffWidth, 0.0);
      this->mpc_ButtonGroup->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle unresolved resize request

   \param[in] orc_NewSize  New size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_ForceWidgetResize(const QSizeF & orc_NewSize)
{
   if (this->mpc_Widget != NULL)
   {
      //Map scene size to widget size
      const QRectF c_CurrentRect = this->mpc_ProxyWidget->subWidgetRect(this->mpc_Widget);
      const float64_t f64_WidthDiff = orc_NewSize.width() - c_CurrentRect.width();
      const int32_t s32_Width = this->mpc_Widget->width() + static_cast<int32_t>(f64_WidthDiff);
      const float64_t f64_HeightDiff = orc_NewSize.height() - c_CurrentRect.height();
      const int32_t s32_Height = this->mpc_Widget->height() + static_cast<int32_t>(f64_HeightDiff);
      const QSize c_New = QSize(std::max(s32_Width, 1), std::max(s32_Height, 1));

      //Force use of new size
      this->mpc_Widget->setFixedSize(c_New);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   Here: Deactivating the edit content mode when deselecting the element

   \param[in] oe_Change Indicator what changed
   \param[in] orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiSvDaRectBaseGroup::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   if ((oe_Change == ItemSelectedHasChanged) &&
       (orc_Value == false))
   {
      // In case of deselecting the element, deactivate the edit content mode in all cases
      this->DisableEditContent();
   }

   return C_GiBiRectBaseGroup::itemChange(oe_Change, orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   bool q_GenericButtonClicked = false;

   // Special case: Check for generic button
   if ((this->mq_ConnectionActive == true) &&
       (this->mpc_ButtonGroup != NULL) &&
       (this->mpc_ButtonGroup->isVisible() == true))
   {
      const QPointF c_Pos = opc_Event->scenePos();
      const QRectF c_Rect = this->mpc_ButtonGroup->sceneBoundingRect();

      // is the cursor in the area of the label
      if (c_Rect.contains(c_Pos) == true)
      {
         m_HandleGenericButtonClick();
         q_GenericButtonClicked = true;
      }
   }

   if (q_GenericButtonClicked == false)
   {
      // Interactions with the inner widget only in deactivated edit mode or the activated specific edit content mode
      if (((this->mq_EditContentModeEnabled == true) || (this->mq_EditModeActive == false)) &&
          (this->IsMousePosRelevantForProxyWidgetInteraction(opc_Event->scenePos()) == true))
      {
         //Start proxy widget interaction
         this->mq_ProxyWidgetInteractionActive = true;
         Q_EMIT (this->SigWidgetHandling(true));
         if (this->isSelected() == false)
         {
            // Select if only not already selected
            Q_EMIT (this->SigSelected(this, true));
         }

         this->mpc_ProxyWidget->TriggerMousePressEvent(opc_Event);
         opc_Event->accept();
      }
      else
      {
         C_GiBiRectBaseGroup::mousePressEvent(opc_Event);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   //Always handle the mouse move event the same way as the mouse press event was handled
   if (this->mq_ProxyWidgetInteractionActive == true)
   {
      this->mpc_ProxyWidget->TriggerMouseMoveEvent(opc_Event);
      opc_Event->accept();
   }
   else
   {
      C_GiBiRectBaseGroup::mouseMoveEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   //Always handle the mouse release event the same way as the mouse press event was handled
   if (this->mq_ProxyWidgetInteractionActive == true)
   {
      this->mpc_ProxyWidget->TriggerMouseReleaseEvent(opc_Event);
      opc_Event->accept();
   }
   else
   {
      C_GiBiRectBaseGroup::mouseReleaseEvent(opc_Event);
   }
   //End proxy widget interaction
   Q_EMIT (this->SigWidgetHandling(false));
   this->mq_ProxyWidgetInteractionActive = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Send event to proxy widget in content edit mode

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if ((this->mq_EditModeActive == false) ||
       (this->mq_EditContentModeEnabled == true))
   {
      //Start proxy widget interaction
      this->mq_ProxyWidgetInteractionActive = true;

      this->mpc_ProxyWidget->TriggerMouseDoubleClickEvent(opc_Event);
      opc_Event->accept();
   }
   else
   {
      C_GiBiRectBaseGroup::mouseDoubleClickEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape)) &&
       (this->mq_EditContentModeEnabled == true))
   {
      this->DisableEditContent();
      opc_Event->accept();
   }
   // Interactions with the inner widget only in deactivated edit mode or the activated specific edit content mode
   else if ((this->mq_EditModeActive == false) ||
            (this->mq_EditContentModeEnabled == true))
   {
      this->mpc_ProxyWidget->TriggerKeyPressEvent(opc_Event);
      opc_Event->accept();
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key release event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::keyReleaseEvent(QKeyEvent * const opc_Event)
{
   // Interactions with the inner widget only in deactivated edit mode or the activated specific edit content mode
   if ((this->mq_EditModeActive == false) ||
       (this->mq_EditContentModeEnabled == true))
   {
      this->mpc_ProxyWidget->TriggerKeyReleaseEvent(opc_Event);
      opc_Event->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus out event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::focusOutEvent(QFocusEvent * const opc_Event)
{
   this->mpc_ProxyWidget->TriggerFocusOutEvent(opc_Event);
   C_GiBiRectBaseGroup::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten wheel event slot

   Here: Send event to proxy widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::wheelEvent(QGraphicsSceneWheelEvent * const opc_Event)
{
   // Interactions with the inner widget only in deactivated edit mode or the activated specific edit content mode
   if (((this->mq_EditModeActive == false) ||
        (this->mq_EditContentModeEnabled == true)) &&
       (this->mpc_ProxyWidget->hasFocus() == true))
   {
      this->mpc_ProxyWidget->TriggerWheelEvent(opc_Event);
      //If widget does not handle scroll event, forward the signal
      if (opc_Event->isAccepted() == false)
      {
         C_GiBiRectBaseGroup::wheelEvent(opc_Event);
      }
   }
   else
   {
      C_GiBiRectBaseGroup::wheelEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover enter event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::hoverEnterEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   this->mpc_ProxyWidget->TriggerHoverEnterEvent(opc_Event);
   C_GiBiRectBaseGroup::hoverEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover move event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   this->mpc_ProxyWidget->TriggerHoverMoveEvent(opc_Event);

   C_GiBiRectBaseGroup::hoverMoveEvent(opc_Event);

   //Handle tool tips
   if ((this->mpc_RefreshIcon->isVisible() == true) &&
       (this->mpc_RefreshIcon->contains(this->mpc_RefreshIcon->mapFromScene(opc_Event->scenePos())) == true))
   {
      QString c_ManualItems;
      QString c_Text;
      if (this->mq_ReadItem == true)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            for (uint32_t u32_ItItem = 0; u32_ItItem < this->GetWidgetDataPoolElementCount(); ++u32_ItItem)
            {
               C_PuiSvDbNodeDataPoolListElementId c_Id;
               if ((this->GetDataPoolElementIndex(u32_ItItem, c_Id) == C_NO_ERR) && (c_Id.GetIsValid() == true))
               {
                  C_PuiSvReadDataConfiguration c_Config;
                  if (pc_View->GetReadRailAssignment(c_Id, c_Config) == C_NO_ERR)
                  {
                     if (c_Config.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
                     {
                        const C_OscNodeDataPoolListElement * const pc_Element =
                           C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id);
                        if (pc_Element != NULL)
                        {
                           //Add name
                           if (c_Id.GetUseArrayElementIndex())
                           {
                              c_ManualItems += static_cast<QString>("%1[%2]").arg(pc_Element->c_Name.c_str()).arg(
                                 c_Id.GetArrayElementIndex());
                           }
                           else
                           {
                              c_ManualItems += static_cast<QString>(pc_Element->c_Name.c_str());
                           }
                           c_ManualItems += "\n";
                        }
                     }
                  }
               }
            }
         }
         c_Text = C_GtGetText::h_GetText("Trigger manual read for:\n");
      }
      else
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            for (uint32_t u32_ItItem = 0; u32_ItItem < this->GetWidgetDataPoolElementCount(); ++u32_ItItem)
            {
               C_PuiSvDbNodeDataPoolListElementId c_Id;
               if ((this->GetDataPoolElementIndex(u32_ItItem, c_Id) == C_NO_ERR) && (c_Id.GetIsValid() == true))
               {
                  const C_OscNodeDataPoolListElement * const pc_Element =
                     C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id);
                  if (pc_Element != NULL)
                  {
                     //Add name
                     if (c_Id.GetUseArrayElementIndex())
                     {
                        c_ManualItems += static_cast<QString>("%1[%2]").arg(pc_Element->c_Name.c_str()).arg(
                           c_Id.GetArrayElementIndex());
                     }
                     else
                     {
                        c_ManualItems += static_cast<QString>(pc_Element->c_Name.c_str());
                     }
                     c_ManualItems += "\n";
                  }
               }
            }
         }
         c_Text = C_GtGetText::h_GetText("Trigger manual write for:\n");
      }
      c_Text += c_ManualItems;
      if (this->mq_ConnectionActive == false)
      {
         c_Text += C_GtGetText::h_GetText("\nOnly available while connected.");
      }
      //Check if redisplay necessary
      if (c_Text.compare(this->GetCurrentToolTipContent()) != 0)
      {
         Q_EMIT this->SigHideToolTip();
      }
      this->SetDefaultToolTipHeading("");
      this->SetDefaultToolTipContent(c_Text);
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
   }
   else if (((this->mpc_WarningIcon != NULL) && (this->mpc_WarningIcon->isVisible() == true)) &&
            (this->mpc_WarningIcon->contains(this->mpc_WarningIcon->mapFromScene(opc_Event->scenePos())) == true))
   {
      const QString c_Heading = C_GtGetText::h_GetText("Configuration Warning");
      QString c_Content;
      QString c_Name;
      //if there are no data elements use this tooltip
      if (this->GetWidgetDataPoolElementCount() == 0U)
      {
         c_Content += C_GtGetText::h_GetText("- No data element selected");
      }
      else
      {
         if (m_CheckHasValidElements(c_Name) == false)
         {
            c_Content +=
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "- This widget and this data element (\"%1\") does not match, "
                                       "possible reasons:\n"
                                       "   Data element was deleted\n"
                                       "   Data element has become an array\n"
                                       "   Data element has different value range")).arg(c_Name);
         }
         if (m_CheckHasAnyRequiredNodesActive() == false)
         {
            if (c_Content.isEmpty() == false)
            {
               c_Content += "\n";
            }
            c_Content += C_GtGetText::h_GetText("- There is a data element of an inactive node");
         }
         if (m_CheckHasAnyRequiredBusesConnected() == false)
         {
            if (c_Content.isEmpty() == false)
            {
               c_Content += "\n";
            }
            c_Content += C_GtGetText::h_GetText("- There is a signal of an inactive bus");
         }
         if (m_CheckHasAnyRequiredNodesValidDashboardRouting() == false)
         {
            if (c_Content.isEmpty() == false)
            {
               c_Content += "\n";
            }
            c_Content += C_GtGetText::h_GetText("- There is a data element of a node with "
                                                "disabled communication interface flags for Dashboard");
         }
      }
      //Check if redisplay necessary
      if (c_Content.compare(this->GetCurrentToolTipContent()) != 0)
      {
         Q_EMIT (this->SigHideToolTip());
      }
      this->SetDefaultToolTipHeading(c_Heading);
      this->SetDefaultToolTipContent(c_Content);
      this->SetDefaultToolTipType(C_NagToolTip::eWARNING);
   }
   else if (((this->mpc_ConflictIcon != NULL) && (this->mpc_ConflictIcon->isVisible() == true)) &&
            (this->mpc_ConflictIcon->contains(this->mpc_ConflictIcon->mapFromScene(opc_Event->scenePos())) == true))
   {
      m_UpdateErrorIconToolTip();
   }
   else
   {
      C_PuiSvDbNodeDataPoolListElementId c_Id;
      QString c_Heading = "";
      const QString c_NewContent = m_GetCommonToolTipContent();
      //Check if redisplay necessary
      if (c_NewContent.compare(this->GetCurrentToolTipContent()) != 0)
      {
         Q_EMIT this->SigHideToolTip();
      }

      //get element name as heading
      if (c_NewContent.isEmpty() == false)
      {
         if (this->GetDataPoolElementIndex(0, c_Id) == C_NO_ERR)
         {
            if (C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id) != NULL)
            {
               const QString c_ElementName =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id)->c_Name.c_str();
               if (c_Id.GetUseArrayElementIndex())
               {
                  c_Heading = static_cast<QString>("%1[%2]").arg(c_ElementName).arg(c_Id.GetArrayElementIndex());
               }
               else
               {
                  c_Heading = c_ElementName;
               }
            }
         }
      }

      this->SetDefaultToolTipHeading(c_Heading);
      this->SetDefaultToolTipContent(c_NewContent);
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse hover leave event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   // Handle the leave event always to prevent problems with tooltips
   this->mpc_ProxyWidget->TriggerHoverLeaveEvent(opc_Event);
   C_GiBiRectBaseGroup::hoverLeaveEvent(opc_Event);

   Q_EMIT (this->SigHideToolTip());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Event function if the datapool element configuration was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_DataPoolElementsChanged(void)
{
   bool q_InvalidElement;

   if (this->GetWidgetDataPoolElementCount() > 0U)
   {
      // Check for invalid elements
      QString c_Name;
      const bool q_ManualReadElement = m_CheckManualReadRequired();

      q_InvalidElement = (m_CheckHasValidElements(c_Name) == false) ||
                         (m_CheckHasAnyRequiredNodesActive() == false) ||
                         (m_CheckHasAnyRequiredBusesConnected() == false) ||
                         (m_CheckHasAnyRequiredNodesValidDashboardRouting() == false);

      if (((((this->me_WriteMode == C_PuiSvDbWidgetBase::eWM_MANUAL) && (this->mq_ReadItem == false)) ||
            (q_ManualReadElement == true)) &&
           (q_InvalidElement == false)) && (this->m_AllowRefreshButton() == true))
      {
         // Check if element is a writing element with manual update or a reading element with manual trigger
         this->mq_ShowButton = true;
      }
      else
      {
         this->mq_ShowButton = false;
      }
   }
   else
   {
      q_InvalidElement = true;
      this->mq_ShowButton = false;
   }

   if (this->mq_ReadItem == true)
   {
      this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_READ_DISABLED);
   }
   else
   {
      this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_DISABLED);
   }
   if (this->mpc_ButtonGroup != NULL)
   {
      this->mpc_ButtonGroup->setVisible(this->mq_ShowButton);
   }

   if ((this->mpc_WarningIcon != NULL) && (m_AllowWarningIcon() == true))
   {
      this->mpc_WarningIcon->setVisible(q_InvalidElement);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if the refresh button is available

   \return
   True  Refresh icon will be displayed if necessary
   False Refresh icon will never be displayed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::m_AllowRefreshButton(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_UpdateErrorIcon(void)
{
   if ((this->mpc_ConflictIcon != NULL) && (this->mpc_ButtonGroup != NULL))
   {
      if ((this->mc_CommmunicationErrors.size() > 0) ||
          (this->mc_InvalidDlcSignals.size() > 0))
      {
         this->mpc_ConflictIcon->setVisible(true);
         if (this->mpc_ButtonGroup->isVisible())
         {
            //Don't use move (cumulative) use set pos instead
            //Move icon below update trigger
            this->mpc_ConflictIcon->setPos(this->mpc_ConflictIcon->x(), this->mpc_ButtonGroup->boundingRect().height());
         }
      }
      else
      {
         this->mpc_ConflictIcon->setVisible(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if warning icon is allowed

   \return
   True  Warning icon is allowed
   False Warning icon is not allowed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::m_AllowWarningIcon(void) const
{
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if element supports the edit content mode

   Default elements does not have the widget specific edit content mode.
   If an element support this mode, the derived class must reimplement this function.

   \return
   True  Element does have and support a widget specific edit content mode
   False Element does not have and support a widget specific edit content mode
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaRectBaseGroup::m_HasEditContentMode(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get common tool tip content if no other item takes precedence over the tool tip

   \return
   Common tool tip content if no other item takes precedence over the tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiSvDaRectBaseGroup::m_GetCommonToolTipContent(void) const
{
   QString c_Retval;
   C_PuiSvDbNodeDataPoolListElementId c_Id;

   if (this->GetDataPoolElementIndex(0, c_Id) == C_NO_ERR)
   {
      c_Retval = C_SyvUtil::h_GetCommonDashboardItemToolTip(this->mu32_ViewIndex, c_Id, this->mq_ReadItem,
                                                            this->me_WriteMode);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_InitConflictIcon(void)
{
   //Offset to have icon inside widget
   const float64_t f64_PosHorizontal =
      (this->GetVisibleBoundingRect().width() - (static_cast<float64_t>(this->ms32_IconSize) * 1.8125));
   const float64_t f64_POS_Y = 0.0;

   // create the conflict icon
   mpc_ConflictIcon =
      new C_GiSvgGraphicsItem("://images/Error_iconV2.svg", static_cast<float64_t>(this->ms32_IconSize),
                              static_cast<float64_t>(this->ms32_IconSize));

   this->mpc_ConflictIcon->moveBy(f64_PosHorizontal - this->mpc_ConflictIcon->pos().x(),
                                  f64_POS_Y - this->mpc_ConflictIcon->pos().y());

   // set the position of the icon
   this->mpc_ConflictIcon->setParentItem(this);

   // the icon will be shown if a conflict is detected
   this->mpc_ConflictIcon->setVisible(false);

   // create the warning icon
   this->mpc_WarningIcon =
      new C_GiSvgGraphicsItem("://images/system_views/IconConfigWarning.svg",
                              static_cast<float64_t>(this->ms32_IconSize),
                              static_cast<float64_t>(this->ms32_IconSize));

   this->mpc_WarningIcon->moveBy(f64_PosHorizontal - this->mpc_WarningIcon->pos().x(),
                                 f64_POS_Y - this->mpc_WarningIcon->pos().y());

   // set the position of the icon
   this->mpc_WarningIcon->setParentItem(this);

   // the icon will be shown if a warning is detected
   this->mpc_WarningIcon->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_InitButton(void)
{
   QRectF c_Rect;
   QString c_Path;
   QGraphicsRectItem * pc_RectItem;
   //Offset to have icon inside widget
   const float64_t f64_PosHorizontal =
      (this->GetVisibleBoundingRect().width() - (static_cast<float64_t>(this->ms32_IconSize) * 1.8125));
   const float64_t f64_POS_Y = 0.0;

   // create 'button' with points
   mpc_ButtonGroup = new QGraphicsItemGroup();
   this->mpc_ButtonGroup->setCursor(Qt::ArrowCursor);

   if (this->mq_ReadItem == true)
   {
      c_Path = C_GiSvDaRectBaseGroup::mhc_ICON_READ_DISABLED;
   }
   else
   {
      c_Path = C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_DISABLED;
   }
   // create the refresh icon
   mpc_RefreshIcon = new C_GiSvgGraphicsItem(c_Path, static_cast<float64_t>(this->ms32_IconSize),
                                             static_cast<float64_t>(this->ms32_IconSize));

   this->mpc_RefreshIcon->moveBy(f64_PosHorizontal - this->mpc_RefreshIcon->pos().x(),
                                 f64_POS_Y - this->mpc_RefreshIcon->pos().y());

   this->mpc_ButtonGroup->addToGroup(this->mpc_RefreshIcon);

   //Increase bounding rect
   c_Rect = this->mpc_ButtonGroup->boundingRect();
   c_Rect.setX(f64_PosHorizontal);
   c_Rect.setY(f64_POS_Y);
   c_Rect.setWidth(static_cast<float64_t>(this->ms32_IconSize));
   c_Rect.setHeight(static_cast<float64_t>(this->ms32_IconSize));
   pc_RectItem = new QGraphicsRectItem(c_Rect);
   pc_RectItem->setPen(static_cast<QPen>(Qt::NoPen));
   pc_RectItem->setBrush(static_cast<QBrush>(Qt::NoBrush));
   this->mpc_ButtonGroup->addToGroup(pc_RectItem);

   this->mpc_ButtonGroup->setParentItem(this);
   this->mpc_ButtonGroup->setVisible(false);
} //lint !e429  //no memory leak because of adding pc_RectItem to the group and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger next manual read operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_ManualRead(void)
{
   if ((this->mu32_NextManualActionIndex == 0) && (this->mpc_RefreshIcon != NULL))
   {
      this->m_ManualOperationStarted();
   }
   if ((this->GetWidgetDataPoolElementCount() > this->mu32_NextManualActionIndex) && (this->mq_ReadItem == true))
   {
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;

      const int32_t s32_Return = this->GetDataPoolElementIndex(this->mu32_NextManualActionIndex, c_ElementId);
      tgl_assert(s32_Return == C_NO_ERR);
      if (s32_Return == C_NO_ERR)
      {
         //Always iterate to next element!
         ++this->mu32_NextManualActionIndex;
         if (this->m_CheckIsOnTrigger(c_ElementId))
         {
            if ((c_ElementId.GetIsValid() == true) &&
                (this->m_CheckNodeActive(c_ElementId.u32_NodeIndex) == true) &&
                (this->m_CheckNodeHasAnyRequiredValidDashboardRouting(c_ElementId.u32_NodeIndex) == true))
            {
               //-1 because we already prepared for the next element!
               if (this->m_CheckElementAlreadyRead(this->mu32_NextManualActionIndex - 1UL, c_ElementId) == false)
               {
                  Q_EMIT (this->SigDataPoolRead(c_ElementId));
               }
               else
               {
                  //Skip to next element
                  this->m_ManualRead();
               }
            }
            else
            {
               //Skip to next element
               this->m_ManualRead();
            }
         }
         else
         {
            //Skip to next element
            this->m_ManualRead();
         }
      }
      else
      {
         //Unexpected
         this->m_ManualOperationFinished();
      }
   }
   else
   {
      //Last element reached or write element
      this->m_ManualOperationFinished();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle any click to the generic button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_HandleGenericButtonClick(void)
{
   this->mq_AbortTriggered = false;
   //Read the read element and write the value of the write element
   if (this->mq_ReadItem == false)
   {
      // Write the actual value to the server
      this->SendCurrentValue();
   }
   else
   {
      //Only for read items this may be abort
      if (this->mu32_NextManualActionIndex > 0UL)
      {
         //Register abort
         this->mq_AbortTriggered = true;
      }
      else
      {
         this->m_ManualRead();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_UpdateErrorIconToolTip(void)
{
   QString c_Heading = C_GtGetText::h_GetText("Transmission error");
   QString c_Content;

   if (this->mc_CommmunicationErrors.size() > 0L)
   {
      c_Content += C_GtGetText::h_GetText("Following transmissions failed:\n");
      for (QMap<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString>::const_iterator c_It =
              this->mc_CommmunicationErrors.begin();
           c_It != this->mc_CommmunicationErrors.end(); ++c_It)
      {
         if (c_It.key().GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
         {
            c_Content += static_cast<QString>(C_GtGetText::h_GetText("%1 encountered error: %2")).
                         arg(C_PuiSdUtil::h_GetNamespace(c_It.key())).arg(c_It.value());
         }
         else
         {
            c_Content += static_cast<QString>(C_GtGetText::h_GetText("%1 encountered error: %2")).
                         arg(C_PuiSdUtil::h_GetSignalNamespace(c_It.key())).arg(c_It.value());
         }
      }
   }
   if (c_Content.isEmpty() == true)
   {
      if (this->mc_InvalidDlcSignals.size() > 0L)
      {
         c_Heading = C_GtGetText::h_GetText("Signal error");
         for (QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, uint8_t>::const_iterator c_It =
                 this->mc_InvalidDlcSignals.begin();
              c_It != this->mc_InvalidDlcSignals.end(); ++c_It)
         {
            c_Content += static_cast<QString>(C_GtGetText::h_GetText("%1 had invalid DLC %2.")).
                         arg(C_PuiSdUtil::h_GetSignalNamespace(c_It.key())).arg(QString::number(c_It.value()));
         }
      }
   }
   //Check if redisplay necessary
   if (c_Content.compare(this->GetCurrentToolTipContent()) != 0)
   {
      Q_EMIT this->SigHideToolTip();
   }
   this->SetDefaultToolTipHeading(c_Heading);
   this->SetDefaultToolTipContent(c_Content);
   this->SetDefaultToolTipType(C_NagToolTip::eERROR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   First step of manual operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_ManualOperationStarted(void)
{
   if ((this->mpc_RefreshIcon != NULL) && (this->GetWidgetDataPoolElementCount() > 1))
   {
      if (this->mq_ReadItem == true)
      {
         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_READ_ABORT);
      }
      else
      {
         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_WRITE_ABORT);
      }
   }
   QApplication::setOverrideCursor(Qt::WaitCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Final step of manual operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaRectBaseGroup::m_ManualOperationFinished(void)
{
   //Reset current state
   this->mu32_NextManualActionIndex = 0;
   if ((this->mpc_RefreshIcon != NULL) && (this->GetWidgetDataPoolElementCount() > 1))
   {
      if (this->mq_ReadItem == true)
      {
         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_READ);
      }
      else
      {
         this->mpc_RefreshIcon->SetSvg(C_GiSvDaRectBaseGroup::mhc_ICON_WRITE);
      }
   }
   QApplication::restoreOverrideCursor();
}
