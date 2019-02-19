//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for system view dashboard label item (implementation)

   Class for system view dashboard label item

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_SyvDaPeBase.h"
#include "C_SyvDaPeLabel.h"
#include "C_GiSvDaLabelBase.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of data element in dashboard in system view
   \param[in]     oru64_ID             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaLabelBase::C_GiSvDaLabelBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                     const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                     QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eLABEL, 1,
                         oru64_ID, 35.0, 25.6, 180.0, 100.0, false, true, opc_Parent)
{
   this->mpc_LabelWidget = new C_SyvDaItDashboardLabelWidget();
   this->mpc_Widget->SetWidget(this->mpc_LabelWidget);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaLabelBase::~C_GiSvDaLabelBase(void)
{
   //lint -e{1540}  no memory leak because of the parent of mpc_LabelWidget by calling SetWidget and the Qt memory
   // management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this item

   \return  ID

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_GiSvDaLabelBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_LABEL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();
      if (pc_Dashboard != NULL)
      {
         const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->mpc_LabelWidget->SetDisplayStyle(pc_Box->e_Type, pc_Box->q_ShowCaption, pc_Box->q_ShowUnit);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     17.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::ReInitializeSize(void)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->AdjustFontToSize();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load data from system view dashboard

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32>(this->ms32_Index));
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

//-----------------------------------------------------------------------------
/*!
   \brief   Update data in system view dashboard

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbLabel c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system view dashboard

   \created     19.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the shown value of the element

   \created     04.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateShowValue(void)
{
   float64 f64_Value;

   if (this->m_GetLastValue(0, f64_Value, false) == C_NO_ERR)
   {
      if (this->mpc_LabelWidget != NULL)
      {
         const QString c_Text = this->GetUnscaledValueAsScaledString(f64_Value);
         this->mpc_LabelWidget->SetValue(c_Text);
      }
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update of the color transparence value configured by the actual timeout state

   Base class implementation does nothing. If the dashboard element has drawing elements which
   can not be adapted by stylesheets the derived class must reimplement this function.

   \param[in]     ou32_WidgetDataPoolElementIndex     Index of shown datapool element in widget
   \param[in]     osn_Value                           Value for transparence (0..255)

   \created     18.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   C_GiSvDaRectBaseGroup::UpdateTransparence(ou32_DataElementIndex, osn_Value);
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->ResetFont();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of a connection

   \param[in]  oq_Active Flag if connection is active or not active now

   \created     04.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Call properties for widgets

   \created     04.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaLabelBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbLabel * const pc_Box = pc_Dashboard->GetLabel(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeLabel * pc_PropertiesWidget;
         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Value Label",
                                       c_ElementId,
                                       c_Scaling, true, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeLabel(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(QSize(800, 800));

         pc_PropertiesWidget->SetCaption(pc_Box->c_Caption);
         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowCaption(pc_Box->q_ShowCaption);
         pc_PropertiesWidget->SetShowUnit(pc_Box->q_ShowUnit);

         if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
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
            c_Box.c_DataPoolElementsConfig.clear();
            c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);

            //Force update
            this->mq_InitialStyleCall = true;
            //Apply
            this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
            m_UpdateCaption(c_Box);
            this->UpdateTypePe(c_Box.e_Type, c_Box.q_ShowCaption, c_Box.q_ShowUnit);
            m_UpdateStaticValue(c_Box);
            this->ClearDataPoolElements();
            this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(), pc_Dialog->GetScalingInformation());

            tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           static_cast<uint32>(this->ms32_Index),
                                                                           &c_Box, this->me_Type) == C_NO_ERR);
            //Signal for error change
            Q_EMIT this->SigDataElementsChanged();
         }
         Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
         if (c_New != NULL)
         {
            c_New->HideOverlay();
         }
         //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
      }
   }
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update type

   Warning: Only use for preview

   \param[in] oe_Type        Type
   \param[in] oq_ShowCaption Show caption
   \param[in] oq_ShowUnit    Show unit

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::UpdateTypePe(const C_PuiSvDbLabel::E_Type oe_Type, const bool oq_ShowCaption,
                                     const bool oq_ShowUnit)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetDisplayStyle(oe_Type, oq_ShowCaption, oq_ShowUnit);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set caption

   Warning: Only use for preview

   \param[in] orc_Value New caption

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetCaptionPe(const QString & orc_Value) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetCaption(orc_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set value

   Warning: Only use for preview

   \param[in] orc_Value New value

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetValuePe(const QString & orc_Value) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetValue(orc_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set unit

   Warning: Only use for preview

   \param[in] orc_Value New unit

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::SetUnitPe(const QString & orc_Value)
{
   if (this->mpc_LabelWidget != NULL)
   {
      this->mpc_LabelWidget->SetUnit(orc_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply caption

   \param[in] orc_Data Current data

   \created     18.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::m_UpdateCaption(const C_PuiSvDbLabel & orc_Data) const
{
   if (this->mpc_LabelWidget != NULL)
   {
      if ((orc_Data.c_DataPoolElementsConfig.size() > 0UL) && (orc_Data.c_Caption.compare("Value-label") == 0))
      {
         const C_PuiSvDbNodeDataElementConfig & rc_CurItem = orc_Data.c_DataPoolElementsConfig[0];
         if (rc_CurItem.c_ElementId.GetIsValid() == true)
         {
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_CurItem.c_ElementId.u32_NodeIndex,
                                                                          rc_CurItem.c_ElementId.u32_DataPoolIndex,
                                                                          rc_CurItem.c_ElementId.u32_ListIndex,
                                                                          rc_CurItem.c_ElementId.u32_ElementIndex);
            if (pc_Element != NULL)
            {
               this->mpc_LabelWidget->SetCaption(pc_Element->c_Name.c_str());
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

//-----------------------------------------------------------------------------
/*!
   \brief   Update static (= not connected) display label

   Warning: Always call AFTER setting show unit flag

   \param[in] orc_Label Current label item

   \created     29.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaLabelBase::m_UpdateStaticValue(const stw_opensyde_gui_logic::C_PuiSvDbLabel & orc_Label)
{
   if (this->mpc_LabelWidget != NULL)
   {
      QString c_Text = QString::number(0.0);

      if (orc_Label.c_DataPoolElementsConfig.size() > 0)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = orc_Label.c_DataPoolElementsConfig[0];
         if (rc_Config.c_ElementId.GetIsValid() == true)
         {
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                          rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                          rc_Config.c_ElementId.u32_ListIndex,
                                                                          rc_Config.c_ElementId.u32_ElementIndex);
            if (pc_Element != NULL)
            {
               float64 f64_Value;
               pc_Element->c_MinValue.GetAnyValueAsFloat64(f64_Value, 0);
               c_Text = this->GetUnscaledValueInRangeAsScaledString(0.0);
               this->mpc_LabelWidget->SetUnit(pc_Element->c_Unit.c_str());
            }
         }
      }
      this->mpc_LabelWidget->SetValue(c_Text);
   }
}
