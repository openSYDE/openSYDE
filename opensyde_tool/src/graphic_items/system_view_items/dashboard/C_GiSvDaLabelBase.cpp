//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard label item (implementation)

   Class for system view dashboard label item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsView>

#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_SyvDaPeLabel.hpp"
#include "C_GiSvDaLabelBase.hpp"
#include "C_SdNdeDpContentUtil.hpp"

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

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of data element in dashboard in system view
   \param[in]     oru64_Id             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaLabelBase::C_GiSvDaLabelBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                     const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                     QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eLABEL, 1,
                         oru64_Id, 35.0, 25.6, 180.0, 100.0, false, true, opc_Parent)
{
   this->mpc_LabelWidget = new C_SyvDaItDashboardLabelWidget();
   this->mpc_Widget->SetWidget(this->mpc_LabelWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_LabelWidget by calling SetWidget and the Qt memory
// management
C_GiSvDaLabelBase::~C_GiSvDaLabelBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaLabelBase::type(void) const
{
   return ms32_GRAPHICS_ITEM_DB_LABEL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   // Apply style before calling the base function (required so size call can work properly)
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();
      if (pc_Dashboard != NULL)
      {
         const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32_t>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->mpc_LabelWidget->SetDisplayStyle(pc_Box->e_Type, pc_Box->q_ShowCaption, pc_Box->q_ShowUnit);
         }
      }
   }

   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::ReInitializeSize(void)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->AdjustFontToSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         if (this->mpc_LabelWidget != NULL)
         {
            m_UpdateCaption(*pc_Box);
            this->mpc_LabelWidget->SetDisplayStyle(pc_Box->e_Type, pc_Box->q_ShowCaption, pc_Box->q_ShowUnit);
            m_UpdateStaticValue(*pc_Box);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbLabel c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateShowValue(void)
{
   if (this->mpc_LabelWidget != NULL)
   {
      QString c_Value;
      if (this->m_GetLastValue(0UL, c_Value, NULL) == C_NO_ERR)
      {
         this->mpc_LabelWidget->SetValue(c_Value);
      }
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   Base class implementation does nothing. If the dashboard element has drawing elements which
   can not be adapted by stylesheets the derived class must reimplement this function.

   \param[in]     ou32_DataElementIndex     Index of shown datapool element in widget
   \param[in]     os32_Value                 Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value)
{
   C_GiSvDaRectBaseGroup::UpdateTransparency(ou32_DataElementIndex, os32_Value);
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->ResetFont();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      C_PuiSvDbDataElementScaling c_Scaling;

      if (this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR)
      {
         if (this->mpc_LabelWidget != NULL)
         {
            this->mpc_LabelWidget->SetUnit(c_Scaling.c_Unit);
         }
      }
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaLabelBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeLabel * pc_PropertiesWidget;
         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
            c_FormatterConfig = pc_Box->c_DataPoolElementsConfig[0].c_DisplayFormatter;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL,
                                                             false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Value Label",
                                       c_ElementId,
                                       c_Scaling, true, c_FormatterConfig, true, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeLabel(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(C_SyvDaPeBase::h_GetPopupSizeWithDisplayFormatter());

         pc_PropertiesWidget->SetCaption(pc_Box->c_Caption);
         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowCaption(pc_Box->q_ShowCaption);
         pc_PropertiesWidget->SetShowUnit(pc_Box->q_ShowUnit);

         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            C_PuiSvDbLabel c_Box = *pc_Box;
            C_PuiSvDbNodeDataElementConfig c_Tmp;
            //Save
            this->me_Style = pc_Dialog->GetTheme();
            c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
            c_Box.c_Caption = pc_PropertiesWidget->GetCaption();
            c_Box.e_Type = pc_PropertiesWidget->GetType();
            c_Box.q_ShowCaption = pc_PropertiesWidget->GetShowCaption();
            c_Box.q_ShowUnit = pc_PropertiesWidget->GetShowUnit();

            c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Tmp.c_DisplayFormatter = pc_Dialog->GetFormatterInformation();
            c_Box.c_DataPoolElementsConfig.clear();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
            }

            //Force update
            this->mq_InitialStyleCall = true;
            //Add to Data (before call to SetDisplayStyle)
            tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
            tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           static_cast<uint32_t>(this->ms32_Index),
                                                                           &c_Box, this->me_Type) == C_NO_ERR);
            //Apply
            this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
            m_UpdateCaption(c_Box);
            this->UpdateTypePe(c_Box.e_Type, c_Box.q_ShowCaption, c_Box.q_ShowUnit);
            this->ClearDataPoolElements();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(),
                                             pc_Dialog->GetScalingInformation(), pc_Dialog->GetFormatterInformation());
            }
            m_UpdateStaticValue(c_Box);

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
   \param[in] oq_ShowCaption Show caption
   \param[in] oq_ShowUnit    Show unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateTypePe(const C_PuiSvDbLabel::E_Type oe_Type, const bool oq_ShowCaption,
                                     const bool oq_ShowUnit)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetDisplayStyle(oe_Type, oq_ShowCaption, oq_ShowUnit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set caption

   Warning: Only use for preview

   \param[in] orc_Value New caption
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetCaptionPe(const QString & orc_Value) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetCaption(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   Warning: Only use for preview

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetValuePe(const QString & orc_Value) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetValue(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unit

   Warning: Only use for preview

   \param[in] orc_Value New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetUnitPe(const QString & orc_Value)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetUnit(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply caption

   \param[in] orc_Data Current data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::m_UpdateCaption(const C_PuiSvDbLabel & orc_Data) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      if ((orc_Data.c_DataPoolElementsConfig.size() > 0UL) &&
          ((orc_Data.c_Caption.compare("VALUE-LABEL", Qt::CaseInsensitive) == 0) ||
           (orc_Data.c_Caption == "")))
      {
         const C_PuiSvDbNodeDataElementConfig & rc_CurItem = orc_Data.c_DataPoolElementsConfig[0];
         if (rc_CurItem.c_ElementId.GetIsValid() == true)
         {
            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_CurItem.c_ElementId.u32_NodeIndex,
                                                                          rc_CurItem.c_ElementId.u32_DataPoolIndex,
                                                                          rc_CurItem.c_ElementId.u32_ListIndex,
                                                                          rc_CurItem.c_ElementId.u32_ElementIndex);
            const C_OscNodeDataPool * const pc_Datapool =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPool(rc_CurItem.c_ElementId.u32_NodeIndex,
                                                               rc_CurItem.c_ElementId.u32_DataPoolIndex);

            if (pc_Element != NULL)
            {
               QString c_Caption;
               if (pc_Datapool != NULL)
               {
                  if ((pc_Datapool->e_Type == C_OscNodeDataPool::eHALC) ||
                      (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
                  {
                     c_Caption = C_PuiSvHandler::h_GetShortNamespace(rc_CurItem.c_ElementId);
                  }
                  else
                  {
                     if (rc_CurItem.c_ElementId.GetUseArrayElementIndex())
                     {
                        c_Caption = static_cast<QString>("%1[%2]").arg(pc_Element->c_Name.c_str()).arg(
                           rc_CurItem.c_ElementId.GetArrayElementIndex());
                     }
                     else
                     {
                        c_Caption = pc_Element->c_Name.c_str();
                     }
                  }
               }
               this->mpc_LabelWidget->SetCaption(c_Caption);
            }
            else
            {
               this->mpc_LabelWidget->SetCaption(orc_Data.c_Caption);
            }
         }
         else
         {
            this->mpc_LabelWidget->SetCaption(orc_Data.c_Caption);
         }
      }
      else
      {
         this->mpc_LabelWidget->SetCaption(orc_Data.c_Caption);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update static (= not connected) display label

   Warning: Always call AFTER setting show unit flag

   \param[in] orc_Label Current label item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaLabelBase::m_UpdateStaticValue(const stw::opensyde_gui_logic::C_PuiSvDbLabel & orc_Label)
{
   if (this->mpc_LabelWidget != NULL)
   {
      QString c_Text = QString::number(0.0);

      if (orc_Label.c_DataPoolElementsConfig.size() > 0)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = orc_Label.c_DataPoolElementsConfig[0];
         if (rc_Config.c_ElementId.GetIsValid() == true)
         {
            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                          rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                          rc_Config.c_ElementId.u32_ListIndex,
                                                                          rc_Config.c_ElementId.u32_ElementIndex);
            if (pc_Element != NULL)
            {
               float64_t f64_Value;
               uint32_t u32_Index;
               if (rc_Config.c_ElementId.GetUseArrayElementIndex())
               {
                  u32_Index = rc_Config.c_ElementId.GetArrayElementIndex();
               }
               else
               {
                  u32_Index = 0UL;
               }
               pc_Element->c_MinValue.GetAnyValueAsFloat64(f64_Value, u32_Index);
               c_Text = this->GetUnscaledValueInRangeAsScaledString(0.0);
               this->mpc_LabelWidget->SetUnit(pc_Element->c_Unit.c_str());
            }
         }
      }
      this->mpc_LabelWidget->SetValue(c_Text);
   }
}
