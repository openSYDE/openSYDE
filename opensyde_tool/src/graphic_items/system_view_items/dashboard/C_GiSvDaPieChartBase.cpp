//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard pie chart item (implementation)

   Class for system view dashboard pie chart item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvDaPieChartBase.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeBase.h"
#include "C_SyvDaPePieChart.h"
#include "C_PuiSvDbPieChart.h"
#include "C_SdNdeDpContentUtil.h"

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
C_GiSvDaPieChartBase::C_GiSvDaPieChartBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                           const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                           QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::ePIE_CHART, 1,
                         oru64_ID, 50.0, 50.0, 100.0, 100.0, true, true, opc_Parent),
   mf64_UnscaledMin(0.0),
   mf64_UnscaledMax(100.0)
{
   this->mpc_PieChartWidget = new C_OgeWiDashboardPieChart();
   this->mpc_Widget->SetWidget(this->mpc_PieChartWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_PieChartWidget by calling SetWidget and the Qt memory
// management
C_GiSvDaPieChartBase::~C_GiSvDaPieChartBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaPieChartBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_PIE_CHART;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();
      if (pc_Dashboard != NULL)
      {
         const C_PuiSvDbPieChart * const pc_Box = pc_Dashboard->GetPieChart(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->mpc_PieChartWidget->SetDisplayStyle(oe_Style, oq_DarkMode, pc_Box->q_ShowUnit, pc_Box->q_ShowValue);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::ReInitializeSize(void)
{
   this->mpc_PieChartWidget->ReInitSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbPieChart * const pc_Box = pc_Dashboard->GetPieChart(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         this->mpc_PieChartWidget->SetDisplayStyle(this->me_Style, this->mq_DarkMode, pc_Box->q_ShowUnit,
                                                   pc_Box->q_ShowValue);
         this->m_UpdateStaticValues();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbPieChart * const pc_Box = pc_Dashboard->GetPieChart(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbPieChart c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::UpdateShowValue(void)
{
   float64 f64_Value;

   if (this->m_GetLastValue(0, f64_Value, false) == C_NO_ERR)
   {
      const float64 f64_Progress =
         ((f64_Value - this->mf64_UnscaledMin) / (this->mf64_UnscaledMax - this->mf64_UnscaledMin)) *
         2000000.0;
      const sint32 s32_Progress = static_cast<sint32>(f64_Progress);
      this->mpc_PieChartWidget->SetValue(this->GetUnscaledValueAsScaledString(f64_Value), s32_Progress);
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in]     ou32_DataElementIndex     Index of shown datapool element in widget
   \param[in]     osn_Value                 Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   if ((ou32_DataElementIndex == 0) &&
       (this->mpc_PieChartWidget != NULL))
   {
      // Only one item is supported by this item
      this->mpc_PieChartWidget->SetTransparence(osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->m_UpdateStaticValues();
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaPieChartBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbPieChart * const pc_Box = pc_Dashboard->GetPieChart(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         sint32 s32_Return = C_NO_ERR;
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;

         // Fill the existing data
         if (this->GetWidgetDataPoolElementCount() > 0)
         {
            s32_Return = this->GetDataPoolElementIndex(0U, c_ElementId);

            if (s32_Return == C_NO_ERR)
            {
               s32_Return = this->GetDataPoolElementScaling(0U, c_Scaling);
            }
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL, false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         if (s32_Return == C_NO_ERR)
         {
            QGraphicsView * const pc_View = this->scene()->views().at(0);
            QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
            C_SyvDaPeBase * pc_Dialog;
            C_SyvDaPePieChart * pc_PropertiesWidget;

            pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Pie Chart",
                                          c_ElementId, c_Scaling, true, this->mq_DarkMode);
            pc_PropertiesWidget = new C_SyvDaPePieChart(*pc_Dialog, this->mq_DarkMode);

            pc_Dialog->SetWidget(pc_PropertiesWidget);
            pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

            //Resize
            c_New->SetSize(QSize(800, 800));

            pc_PropertiesWidget->SetShowUnit(pc_Box->q_ShowUnit);
            pc_PropertiesWidget->SetShowValue(pc_Box->q_ShowValue);

            if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               C_PuiSvDbPieChart c_Box = *pc_Box;
               C_PuiSvDbNodeDataElementConfig c_Tmp;
               //Save
               this->me_Style = pc_Dialog->GetTheme();
               c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
               c_Box.q_ShowUnit = pc_PropertiesWidget->GetShowUnit();
               c_Box.q_ShowValue = pc_PropertiesWidget->GetShowValue();

               c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
               c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
               c_Box.c_DataPoolElementsConfig.clear();
               if (c_Tmp.c_ElementId.GetIsValid())
               {
                  c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
               }

               //Force update
               this->mq_InitialStyleCall = true;
               //Apply
               this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
               if (this->mpc_PieChartWidget != NULL)
               {
                  this->mpc_PieChartWidget->SetDisplayStyle(this->me_Style, this->mq_DarkMode, c_Box.q_ShowUnit,
                                                            c_Box.q_ShowValue);
               }
               this->ClearDataPoolElements();
               if (c_Tmp.c_ElementId.GetIsValid())
               {
                  this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(), pc_Dialog->GetScalingInformation());
               }

               tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
               tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                              this->mu32_DashboardIndex,
                                                                              static_cast<uint32>(this->ms32_Index),
                                                                              &c_Box, this->me_Type) == C_NO_ERR);

               this->m_UpdateStaticValues();
            }
            Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
            if (c_New != NULL)
            {
               c_New->HideOverlay();
            }
            ;
         } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
      }
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update type

   Warning: Only use for preview

   \param[in] oq_ShowUnit    Show unit flag
   \param[in] oq_ShowValue   Show value flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::UpdateTypePe(const bool oq_ShowUnit, const bool oq_ShowValue)
{
   if (this->mpc_PieChartWidget != NULL)
   {
      this->mpc_PieChartWidget->SetDisplayStyle(this->me_Style, this->mq_DarkMode, oq_ShowUnit, oq_ShowValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   Warning: Only use for preview

   \param[in] orc_Value             New value
   \param[in] os32_Progress2000000  Value in percent (only 0-2000000 supported)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::SetValuePe(const QString & orc_Value, const sint32 os32_Progress2000000)
{
   if (this->mpc_PieChartWidget != NULL)
   {
      this->mpc_PieChartWidget->SetValue(orc_Value, os32_Progress2000000);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unit

   Warning: Only use for preview

   \param[in] orc_Value New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::SetUnitPe(const QString & orc_Value)
{
   if (this->mpc_PieChartWidget != NULL)
   {
      this->mpc_PieChartWidget->SetUnit(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaPieChartBase::m_UpdateStaticValues(void)
{
   C_PuiSvDbNodeDataPoolListElementId c_ID;
   C_PuiSvDbDataElementScaling c_Scaling;

   if ((this->GetDataPoolElementIndex(0, c_ID) == C_NO_ERR) &&
       (this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR))
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_ID.u32_NodeIndex, c_ID.u32_DataPoolIndex,
                                                                    c_ID.u32_ListIndex, c_ID.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         const uint32 u32_Index = c_ID.GetArrayElementIndexOrZero();
         C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, this->mf64_UnscaledMin, u32_Index);
         C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, this->mf64_UnscaledMax, u32_Index);

         if (this->mpc_PieChartWidget != NULL)
         {
            float64 f64_Progress;
            const QString c_Value = this->GetUnscaledValueInRangeAsScaledString(0.0, 0UL, &f64_Progress);
            this->mpc_PieChartWidget->SetUnit(c_Scaling.c_Unit);
            //Update value
            const float64 f64_Temp = f64_Progress * 2000000.0;
            this->mpc_PieChartWidget->SetValue(c_Value, static_cast<sint32>(f64_Temp));
         }
      }
   }
   else
   {
      if (this->mpc_PieChartWidget != NULL)
      {
         //Update value
         this->mpc_PieChartWidget->SetValue(static_cast<QString>("%1").arg(0.0), 0);
      }
   }
}
