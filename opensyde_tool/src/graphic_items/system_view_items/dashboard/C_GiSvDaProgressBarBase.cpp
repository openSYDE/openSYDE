//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard progress bar item (implementation)

   Class for system view dashboard progress bar item

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
#include "C_GiSvDaProgressBarBase.h"
#include "C_PuiSvDbProgressBar.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeBase.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_SyvDaPeProgressBar.h"

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

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of data element in dashboard in system view
   \param[in]     oru64_ID             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaProgressBarBase::C_GiSvDaProgressBarBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                                 const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                                 QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::ePROGRESS_BAR, 1,
                         oru64_ID, 100.0, 25.0, 150.0, 150.0, false, true, opc_Parent),
   mpc_ProgressBarWidget(new C_SyvDaItDashboardProgressBarWidget()),
   mf64_UnscaledMin(0.0),
   mf64_UnscaledMax(100.0)
{
   this->mpc_Widget->SetWidget(this->mpc_ProgressBarWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ProgressBarWidget and the Qt memory management
C_GiSvDaProgressBarBase::~C_GiSvDaProgressBarBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaProgressBarBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_PROGRESS_BAR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();
      if ((pc_Dashboard != NULL) &&
          (this->mpc_ProgressBarWidget != NULL))
      {
         const C_PuiSvDbProgressBar * const pc_Box =
            pc_Dashboard->GetProgressBar(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->mpc_ProgressBarWidget->SetDisplayStyle(oe_Style, pc_Box->e_Type, pc_Box->e_Alignment, oq_DarkMode);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::ReInitializeSize(void)
{
   if (this->mpc_ProgressBarWidget != NULL)
   {
      this->mpc_ProgressBarWidget->AdjustFontToSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbProgressBar * const pc_Box = pc_Dashboard->GetProgressBar(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         if (this->mpc_ProgressBarWidget != NULL)
         {
            this->mpc_ProgressBarWidget->SetDisplayStyle(pc_Box->e_DisplayStyle, pc_Box->e_Type, pc_Box->e_Alignment,
                                                         this->mq_DarkMode);
            this->mpc_ProgressBarWidget->SetShowMinMax(pc_Box->q_ShowMinMax);
         }
         this->m_UpdateStaticValues();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbProgressBar * const pc_Box = pc_Dashboard->GetProgressBar(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbProgressBar c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        this->me_Type) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::UpdateShowValue(void)
{
   float64 f64_Value;

   if ((this->mpc_ProgressBarWidget != NULL) &&
       (this->m_GetLastValue(0, f64_Value, false) == C_NO_ERR))
   {
      const QString c_Text = GetUnscaledValueAsScaledString(f64_Value);
      const float64 f64_Progress =
         ((f64_Value - this->mf64_UnscaledMin) / (this->mf64_UnscaledMax - this->mf64_UnscaledMin)) * 2000000.0;
      this->mpc_ProgressBarWidget->SetValue(static_cast<sintn>(f64_Progress), c_Text);
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::ConnectionActiveChanged(const bool oq_Active)
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
bool C_GiSvDaProgressBarBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbProgressBar * const pc_Box = pc_Dashboard->GetProgressBar(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeProgressBar * pc_PropertiesWidget;

         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL, false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Bar",
                                       c_ElementId, c_Scaling, true, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeProgressBar(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(QSize(800, 800));

         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetAlignment(pc_Box->e_Alignment);
         pc_PropertiesWidget->SetShowMinMax(pc_Box->q_ShowMinMax);
         pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);

         if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            C_PuiSvDbProgressBar c_Box = *pc_Box;
            C_PuiSvDbNodeDataElementConfig c_Tmp;
            //Save
            this->me_Style = pc_Dialog->GetTheme();
            c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
            c_Box.e_Type = pc_PropertiesWidget->GetType();
            c_Box.e_Alignment = pc_PropertiesWidget->GetAlignment();
            c_Box.q_ShowMinMax = pc_PropertiesWidget->GetShowMinMax();

            c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Box.c_DataPoolElementsConfig.clear();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
            }
            c_Box.e_ElementWriteMode = pc_Dialog->GetWriteMode();

            //Force update
            this->mq_InitialStyleCall = true;
            //Apply
            this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
            this->UpdateTypePe(c_Box.e_Type, c_Box.e_Alignment, c_Box.q_ShowMinMax);
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
            //Signal for error change
            Q_EMIT this->SigDataElementsChanged();
         }
         Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
         if (c_New != NULL)
         {
            c_New->HideOverlay();
            c_New->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update type

   Warning: Only use for preview

   \param[in] oe_Type        Type
   \param[in] oe_Alignment   Alignment
   \param[in] oq_ShowMinMax  Show minimum / maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::UpdateTypePe(const C_PuiSvDbProgressBar::E_Type oe_Type,
                                           const C_PuiSvDbProgressBar::E_Alignment oe_Alignment,
                                           const bool oq_ShowMinMax)
{
   if (this->mpc_ProgressBarWidget != NULL)
   {
      // helping value to remember the old type
      const C_PuiSvDbProgressBar::E_Type e_TypeOld = mpc_ProgressBarWidget->GetType();
      this->mpc_ProgressBarWidget->SetDisplayStyle(this->me_Style, oe_Type, oe_Alignment, this->mq_DarkMode);
      this->mpc_ProgressBarWidget->SetShowMinMax(oq_ShowMinMax);
      // setting up the size of the progressbar on type-change
      switch (oe_Type)
      {
      case C_PuiSvDbProgressBar::eTYPE_1:
         if (e_TypeOld != C_PuiSvDbProgressBar::eTYPE_1)
         {
            this->ApplySizeChange(this->scenePos(), QSizeF(80.0, 65.0));
         }
         break;
      case C_PuiSvDbProgressBar::eTYPE_2:
         if (e_TypeOld == C_PuiSvDbProgressBar::eTYPE_1)
         {
            this->ApplySizeChange(this->scenePos(), QSizeF(160.0, 100.0));
         }
         break;
      case C_PuiSvDbProgressBar::eTYPE_3:
         if (e_TypeOld == C_PuiSvDbProgressBar::eTYPE_1)
         {
            this->ApplySizeChange(this->scenePos(), QSizeF(200.0, 100.0));
         }
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   Warning: Only use for preview

   \param[in] osn_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::SetValuePe(const sintn osn_Value)
{
   if (this->mpc_ProgressBarWidget != NULL)
   {
      this->mpc_ProgressBarWidget->SetValue(osn_Value, QString::number(osn_Value));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]     ou32_DataElementIndex     Index of shown datapool element in widget
   \param[in]     osn_Value                 Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::UpdateTransparency(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   if ((ou32_DataElementIndex == 0) &&
       (this->mpc_ProgressBarWidget != NULL))
   {
      // Only one item is supported by this item
      this->mpc_ProgressBarWidget->SetTransparency(osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaProgressBarBase::m_UpdateStaticValues(void)
{
   if (this->mpc_ProgressBarWidget != NULL)
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
            QString c_Text;
            float64 f64_Value;
            const uint32 u32_Index = c_ID.GetArrayElementIndexOrZero();
            const QString c_Value = this->GetUnscaledValueInRangeAsScaledString(0.0, u32_Index, &f64_Value);
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, this->mf64_UnscaledMin, u32_Index);
            C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, this->mf64_UnscaledMax, u32_Index);
            this->mpc_ProgressBarWidget->SetUnit(c_Scaling.c_Unit);
            C_SdNdeDpContentUtil::h_GetValueAsScaledString(pc_Element->c_MinValue, c_Scaling.f64_Factor,
                                                           c_Scaling.f64_Offset, c_Text, u32_Index);
            this->mpc_ProgressBarWidget->SetMin(0, c_Text);
            C_SdNdeDpContentUtil::h_GetValueAsScaledString(pc_Element->c_MaxValue, c_Scaling.f64_Factor,
                                                           c_Scaling.f64_Offset, c_Text, u32_Index);
            this->mpc_ProgressBarWidget->SetMax(2000000, c_Text);
            //Update value
            const float64 f64_Temp = f64_Value * 2000000.0;
            this->mpc_ProgressBarWidget->SetValue(static_cast<sintn>(f64_Temp), c_Value);
         }
      }
      else
      {
         //Default
         this->mpc_ProgressBarWidget->SetMin(0, QString::number(0));
         this->mpc_ProgressBarWidget->SetMax(2000000, QString::number(99));
         //Update value
         this->mpc_ProgressBarWidget->SetValue(0, QString::number(0));
      }
   }
}
