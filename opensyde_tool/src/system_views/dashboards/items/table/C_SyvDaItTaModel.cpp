//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for dashboard table widget (implementation)

   Table model for dashboard table widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include "C_Uti.hpp"
#include "C_SdUtil.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_SyvUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_NagToolTip.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_SyvDaItTaModel.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
//Cast to integer on some level so this is the allowed maximum
const uint32_t C_SyvDaItTaModel::hu32_MAX_ELEMENTS = static_cast<uint32_t>(std::numeric_limits<int32_t>::max());

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Data      Data storage
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTaModel::C_SyvDaItTaModel(C_PuiSvDbDataElementHandler * const opc_Data, QObject * const opc_Parent) :
   C_TblModelAction(opc_Parent),
   mpc_Data(opc_Data),
   // Default icons
   mc_IconParameter(":/images/system_definition/IconParameter.svg"),
   mc_IconSignal(":/images/system_definition/IconSignal.svg"),
   mc_IconVariable(":/images/system_definition/IconVariable.svg"),
   mc_IconHalInput(":/images/system_definition/NodeEdit/halc/InputSmallActive.svg"),
   mc_IconHalOutput(":/images/system_definition/NodeEdit/halc/OutputSmallActive.svg"),
   mc_IconHalOther(":/images/system_definition/NodeEdit/halc/OtherSmallActive.svg"),
   // Warning icons
   mc_IconParameterWarning("://images/system_definition/IconParameterWarning.svg"),
   mc_IconSignalWarning("://images/system_definition/IconSignalWarning.svg"),
   mc_IconVariableWarning("://images/system_definition/IconVariableWarning.svg"),
   mc_IconHalOtherWarning(":/images/system_definition/NodeEdit/halc/OtherSmallWarning.svg"),
   // Error icons
   mc_IconParameterError("://images/system_definition/IconParameterError.svg"),
   mc_IconSignalError("://images/system_definition/IconSignalError.svg"),
   mc_IconVariableError("://images/system_definition/IconVariableError.svg"),
   mc_IconHalInputError(":/images/system_definition/NodeEdit/halc/InputSmallError.svg"),
   mc_IconHalOutputError(":/images/system_definition/NodeEdit/halc/OutputSmallError.svg"),
   mc_IconHalOtherError(":/images/system_definition/NodeEdit/halc/OtherSmallError.svg")
{
   //Register to allow data changed signal
   qRegisterMetaType<QVector<int32_t> >();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Style       New style type
   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   switch (oe_Style)
   {
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      if (oq_DarkMode == true)
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_2;
      }
      else
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_34;
      }
      break;

   case C_PuiSvDbWidgetBase::eFLAT:
      if (oq_DarkMode == true)
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_0;
      }
      else
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_34;
      }
      break;

   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      if (oq_DarkMode == true)
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_0;
      }
      else
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_34;
      }
      break;

   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      if (oq_DarkMode == true)
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_0;
      }
      else
      {
         this->mc_TextColor = mc_STYLE_GUIDE_COLOR_6;
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reserve items

   \param[in]  ou32_Number    Number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::ReserveItems(const uint32_t ou32_Number)
{
   this->mc_ScaledDisplayDataValues.reserve(ou32_Number);
   this->mc_UnscaledLastDataValues.reserve(ou32_Number);
   this->mc_UnscaledMaxValues.reserve(ou32_Number);
   this->mc_UnscaledMinValues.reserve(ou32_Number);
   this->mc_ArrayItemIndex.reserve(ou32_Number);
   this->mc_Names.reserve(ou32_Number);
   this->mc_Units.reserve(ou32_Number);
   this->mc_InterpretAsStringFlags.reserve(ou32_Number);
   this->mc_ShowPercentage.reserve(ou32_Number);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize min & max values and names for data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::InitMinMaxAndName(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         const uint32_t u32_Count = pc_Item->c_DataPoolElementsConfig.size();

         //Resize
         this->mc_ScaledDisplayDataValues.resize(u32_Count);
         this->mc_UnscaledLastDataValues.resize(u32_Count);
         this->mc_UnscaledMaxValues.resize(u32_Count);
         this->mc_UnscaledMinValues.resize(u32_Count);
         this->mc_ArrayItemIndex.resize(u32_Count);
         this->mc_Names.resize(u32_Count);
         this->mc_Units.resize(u32_Count);
         this->mc_InterpretAsStringFlags.resize(u32_Count);
         this->mc_ShowPercentage.resize(u32_Count);

         //Init
         this->mc_Transparency.resize(u32_Count, 255);

         //Look up
         for (uint32_t u32_ItElement = 0; u32_ItElement < u32_Count; ++u32_ItElement)
         {
            m_InitMinMaxAndNameForItem(*pc_Item, u32_ItElement);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get newest values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateValue(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         const uint32_t u32_Count = pc_TableWidget->GetWidgetDataPoolElementCount();

         //For each unique ID
         for (uint32_t u32_ItElement = 0; u32_ItElement < u32_Count; ++u32_ItElement)
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            if (pc_TableWidget->GetDataPoolElementIndex(u32_ItElement, c_Id) == C_NO_ERR)
            {
               std::vector<float64_t> c_UnscaledValues;
               std::vector<QString> c_ScaledDisplayValues;
               if ((pc_TableWidget->GetLastValueUnscaled(u32_ItElement, c_UnscaledValues,
                                                         c_ScaledDisplayValues) == C_NO_ERR) &&
                   (c_UnscaledValues.size() > 0))
               {
                  //For each table item
                  for (uint32_t u32_ItConfig = 0; u32_ItConfig < pc_Item->c_DataPoolElementsConfig.size();
                       ++u32_ItConfig)
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        pc_Item->c_DataPoolElementsConfig[u32_ItConfig];
                     if (rc_Config.c_ElementId.CheckSameDataElement(c_Id))
                     {
                        //Set unscaled value
                        if (u32_ItConfig < this->mc_UnscaledLastDataValues.size())
                        {
                           this->mc_UnscaledLastDataValues[u32_ItConfig] = c_UnscaledValues;
                        }
                        else
                        {
                           this->mc_UnscaledLastDataValues.resize(static_cast<uint32_t>(u32_ItConfig) + 1U);
                           this->mc_UnscaledLastDataValues[u32_ItConfig] = c_UnscaledValues;
                        }
                        if (u32_ItConfig < this->mc_ScaledDisplayDataValues.size())
                        {
                           this->mc_ScaledDisplayDataValues[u32_ItConfig] = c_ScaledDisplayValues;
                        }
                        else
                        {
                           this->mc_ScaledDisplayDataValues.resize(static_cast<uint32_t>(u32_ItConfig) + 1U);
                           this->mc_ScaledDisplayDataValues[u32_ItConfig] = c_ScaledDisplayValues;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateError(void)
{
   if (this->rowCount() > 0)
   {
      QVector<int32_t> c_Roles;
      c_Roles.push_back(ms32_USER_ROLE_ICON);
      Q_EMIT (this->dataChanged(this->index(0, 0), this->index(this->rowCount() - 1, 0), c_Roles));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]  ou32_DataElementIndex   Index of shown datapool element in widget
   \param[in]  os32_Value              Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id;
         if (pc_TableWidget->GetDataPoolElementIndex(ou32_DataElementIndex, c_Id) == C_NO_ERR)
         {
            for (uint32_t u32_ItConfig = 0; u32_ItConfig < pc_Item->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Item->c_DataPoolElementsConfig[u32_ItConfig];
               if ((rc_Config.c_ElementId == c_Id) && (u32_ItConfig < this->mc_Transparency.size()))
               {
                  const QModelIndex c_Index =
                     this->index(static_cast<int32_t>(u32_ItConfig), h_EnumToColumn(C_SyvDaItTaModel::eVALUE));
                  this->mc_Transparency[u32_ItConfig] = os32_Value;
                  Q_EMIT this->dataChanged(c_Index, c_Index,
                                           QVector<int32_t>() << static_cast<int32_t>(Qt::ForegroundRole));
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item indices

   \param[in]  orc_Indices       Model indices
   \param[in]  orc_ItemIndices   Item indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::GetUniqueRows(const QModelIndexList & orc_Indices, std::vector<uint32_t> & orc_ItemIndices) const
{
   //Step 1: extract rows
   orc_ItemIndices.reserve(orc_Indices.count());
   for (QModelIndexList::const_iterator c_ItIndex = orc_Indices.begin(); c_ItIndex != orc_Indices.end(); ++c_ItIndex)
   {
      if (c_ItIndex->row() >= 0)
      {
         orc_ItemIndices.push_back(static_cast<uint32_t>(c_ItIndex->row()));
      }
   }
   if (orc_ItemIndices.size() > 0)
   {
      //Step 2: Sort rows descending (otherwise the indices must be changed)
      orc_ItemIndices = C_Uti::h_UniquifyAndSortDescending(orc_ItemIndices);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItTaModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                      const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SyvDaItTaModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = "";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eVALUE:
            c_Retval = C_GtGetText::h_GetText("Value");
            break;
         case eBAR:
            c_Retval = C_GtGetText::h_GetText("Bar");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         switch (e_Col)
         {
         case eVALUE:
            c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
            break;
         case eBAR:
            c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
            break;
         case eICON:
         case eNAME:
         default:
            c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
            break;
         }
      }
      else
      {
         //Unknown
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItTaModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
         dynamic_cast<const stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

      if (pc_TableWidget != NULL)
      {
         const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
         if (pc_Item != NULL)
         {
            if (pc_Item->c_DataPoolElementsConfig.size() == 0)
            {
               s32_Retval = 1;
            }
            else
            {
               s32_Retval = pc_Item->c_DataPoolElementsConfig.size();
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItTaModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = 4;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SyvDaItTaModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
      const C_SyvDaItTaModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      const C_PuiSvDbNodeDataPoolListElementId * const pc_DataElementId = this->GetDataPoolElementIndex(u32_Index);
      if (pc_DataElementId != NULL)
      {
         //Generic settings, independent of invalid flag
         if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
         {
            switch (e_Col)
            {
            case eNAME:
               c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
               break;
            case eVALUE:
               c_Retval = static_cast<QVariant>(Qt::AlignHCenter | Qt::AlignVCenter);
               break;
            case eICON:
            case eBAR:
            default:
               break;
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
         {
            //Stylesheets do not allow access of specific rows so we need to set it manually
            switch (e_Col)
            {
            case C_SyvDaItTaModel::eVALUE:
               if (u32_Index < this->mc_Transparency.size())
               {
                  QColor c_Tmp = this->mc_TextColor;
                  c_Tmp.setAlpha(this->mc_Transparency[u32_Index]);
                  c_Retval = c_Tmp;
               }
               else
               {
                  c_Retval = this->mc_TextColor;
               }
               break;
            case C_SyvDaItTaModel::eICON:
            case C_SyvDaItTaModel::eNAME:
            case C_SyvDaItTaModel::eBAR:
            default:
               c_Retval = this->mc_TextColor;
               break;
            }
         }
         else
         {
            if (pc_DataElementId->GetIsValid() == true)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               const stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
                  dynamic_cast<const stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

               if (pc_TableWidget != NULL)
               {
                  //Default
                  if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
                  {
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        c_Retval = "";
                        break;
                     case C_SyvDaItTaModel::eNAME:
                        if (u32_Index < this->mc_Names.size())
                        {
                           c_Retval = this->mc_Names[u32_Index];
                        }
                        break;
                     case C_SyvDaItTaModel::eVALUE:
                        if (u32_Index < this->mc_Units.size())
                        {
                           c_Retval = static_cast<QString>("%1 %2").arg(m_GetValue(u32_Index)).arg(mc_Units[u32_Index]);
                        }
                        break;
                     case C_SyvDaItTaModel::eBAR:
                        c_Retval = "";
                        break;
                     default:
                        break;
                     }
                  }
                  else if (os32_Role == ms32_USER_ROLE_ICON)
                  {
                     const C_OscNodeDataPool * pc_DataPool;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        pc_DataPool =
                           C_PuiSdHandler::h_GetInstance()->GetOscDataPool(pc_DataElementId->u32_NodeIndex,
                                                                           pc_DataElementId->u32_DataPoolIndex);
                        if (pc_DataPool != NULL)
                        {
                           bool q_IsTransmissionError;
                           QString c_Error;
                           QStringList c_Icons;
                           c_Icons.push_back(QString::number(16));
                           if ((pc_TableWidget->GetViewActive(*pc_DataElementId) == false) ||
                               (pc_TableWidget->GetViewDashboardRouteValid(*pc_DataElementId) == false))
                           {
                              switch (pc_DataPool->e_Type)
                              {
                              // first string is normal icon, second string selected icon
                              // (no selected color specified here, so just use same icon)
                              case C_OscNodeDataPool::eDIAG:
                                 c_Icons.append(this->mc_IconVariableWarning);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eNVM:
                                 c_Icons.append(this->mc_IconParameterWarning);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eCOM:
                                 c_Icons.append(this->mc_IconSignalWarning);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eHALC:
                              case C_OscNodeDataPool::eHALC_NVM:
                                 c_Icons.append(this->mc_IconHalOtherWarning); // in warning case no different HAL icons
                                 c_Retval = c_Icons;
                                 break;
                              default:
                                 break;
                              }
                           }
                           else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                   q_IsTransmissionError) == true)
                           {
                              const C_OscHalcDefDomain::E_Category e_Category =
                                 C_PuiSdHandler::h_GetInstance()->GetDomainCategoryFromDpId(
                                    *pc_DataElementId, pc_DataElementId->GetArrayElementIndexOrZero());
                              switch (pc_DataPool->e_Type)
                              {
                              // first string is normal icon, second string selected icon
                              // (no selected color specified here, so just use same icon)
                              case C_OscNodeDataPool::eDIAG:
                                 c_Icons.append(this->mc_IconVariableError);
                                 c_Icons.append(this->mc_IconVariableError);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eNVM:
                                 c_Icons.append(this->mc_IconParameterError);
                                 c_Icons.append(this->mc_IconParameterError);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eCOM:
                                 c_Icons.append(this->mc_IconSignalError);
                                 c_Icons.append(this->mc_IconSignalError);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eHALC:
                              case C_OscNodeDataPool::eHALC_NVM:
                                 switch (e_Category)
                                 {
                                 case C_OscHalcDefDomain::eCA_INPUT:
                                    c_Icons.append(this->mc_IconHalInputError);
                                    c_Icons.append(this->mc_IconHalInputError);
                                    c_Retval = c_Icons;
                                    break;
                                 case C_OscHalcDefDomain::eCA_OUTPUT:
                                    c_Icons.append(this->mc_IconHalOutputError);
                                    c_Icons.append(this->mc_IconHalOutputError);
                                    c_Retval = c_Icons;
                                    break;
                                 case C_OscHalcDefDomain::eCA_OTHER:
                                    c_Icons.append(this->mc_IconHalOtherError);
                                    c_Icons.append(this->mc_IconHalOtherError);
                                    c_Retval = c_Icons;
                                    break;
                                 default:
                                    break;
                                 }
                                 break;
                              default:
                                 break;
                              }
                           }
                           else
                           {
                              const C_OscHalcDefDomain::E_Category e_Category =
                                 C_PuiSdHandler::h_GetInstance()->GetDomainCategoryFromDpId(
                                    *pc_DataElementId, pc_DataElementId->GetArrayElementIndexOrZero());

                              switch (pc_DataPool->e_Type)
                              {
                              // first string is normal icon, second string selected icon
                              // (no selected color specified here, so just use same icon)
                              case C_OscNodeDataPool::eDIAG:
                                 c_Icons.append(this->mc_IconVariable);
                                 c_Icons.append(this->mc_IconVariable);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eNVM:
                                 c_Icons.append(this->mc_IconParameter);
                                 c_Icons.append(this->mc_IconParameter);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eCOM:
                                 c_Icons.append(this->mc_IconSignal);
                                 c_Icons.append(this->mc_IconSignal);
                                 c_Retval = c_Icons;
                                 break;
                              case C_OscNodeDataPool::eHALC:
                              case C_OscNodeDataPool::eHALC_NVM:
                                 switch (e_Category)
                                 {
                                 case C_OscHalcDefDomain::eCA_INPUT:
                                    c_Icons.append(this->mc_IconHalInput);
                                    c_Icons.append(this->mc_IconHalInput);
                                    c_Retval = c_Icons;
                                    break;
                                 case C_OscHalcDefDomain::eCA_OUTPUT:
                                    c_Icons.append(this->mc_IconHalOutput);
                                    c_Icons.append(this->mc_IconHalOutput);
                                    c_Retval = c_Icons;
                                    break;
                                 case C_OscHalcDefDomain::eCA_OTHER:
                                    c_Icons.append(this->mc_IconHalOther);
                                    c_Icons.append(this->mc_IconHalOther);
                                    c_Retval = c_Icons;
                                    break;
                                 default:
                                    break;
                                 }
                                 break;
                              default:
                                 break;
                              }
                           }
                        }
                        break;
                     case C_SyvDaItTaModel::eNAME:
                     case C_SyvDaItTaModel::eVALUE:
                     case C_SyvDaItTaModel::eBAR:
                     default:
                        break;
                     }
                  }
                  else if (os32_Role == static_cast<int32_t>(Qt::EditRole))
                  {
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eBAR:
                        c_Retval = this->m_GetPercentage(u32_Index);
                        break;
                     case C_SyvDaItTaModel::eICON:
                     case C_SyvDaItTaModel::eNAME:
                     case C_SyvDaItTaModel::eVALUE:
                     default:
                        break;
                     }
                  }
                  else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
                  {
                     QString c_Error;

                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        bool q_IsTransmissionError;
                        if ((pc_TableWidget->GetViewActive(*pc_DataElementId) == false) ||
                            (pc_TableWidget->GetViewDashboardRouteValid(*pc_DataElementId) == false))
                        {
                           c_Retval = C_GtGetText::h_GetText("Configuration warning");
                        }
                        else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                q_IsTransmissionError) == true)
                        {
                           if (q_IsTransmissionError == true)
                           {
                              c_Retval = C_GtGetText::h_GetText("Transmission error");
                           }
                           else
                           {
                              c_Retval = C_GtGetText::h_GetText("Signal error");
                           }
                        }
                        else
                        {
                           //No tool tip
                        }
                        break;
                     case C_SyvDaItTaModel::eNAME:
                        //get element name as heading
                        if (C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(*pc_DataElementId) != NULL)
                        {
                           c_Retval =
                              C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(*pc_DataElementId)->c_Name.
                              c_str();
                        }
                        break;
                     case C_SyvDaItTaModel::eVALUE:
                     case C_SyvDaItTaModel::eBAR:
                     default:
                        break;
                     }
                  }
                  else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
                  {
                     QString c_Error;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        bool q_IsTransmissionError;
                        if (pc_TableWidget->GetViewActive(*pc_DataElementId) == false)
                        {
                           if (pc_DataElementId->GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
                           {
                              c_Retval = C_GtGetText::h_GetText("There is a data element of an inactive node");
                           }
                           else
                           {
                              c_Retval = C_GtGetText::h_GetText("There is a signal of a not connected bus");
                           }
                        }
                        else if (pc_TableWidget->GetViewDashboardRouteValid(*pc_DataElementId) == false)
                        {
                           c_Retval = C_GtGetText::h_GetText("There is a data element of a node with "
                                                             "disabled communication interface flags for Dashboard");
                        }
                        else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                q_IsTransmissionError) == true)
                        {
                           c_Retval = c_Error;
                        }
                        else
                        {
                           //No tool tip
                        }
                        break;
                     case C_SyvDaItTaModel::eNAME:
                        c_Retval = C_SyvUtil::h_GetCommonDashboardItemToolTip(
                           pc_TableWidget->GetViewIndex(), *pc_DataElementId);
                        break;
                     case C_SyvDaItTaModel::eVALUE:
                        if ((u32_Index < this->mc_InterpretAsStringFlags.size()) &&
                            (u32_Index < this->mc_UnscaledLastDataValues.size()))
                        {
                           if ((this->mc_InterpretAsStringFlags[u32_Index] == false) &&
                               (this->mc_UnscaledLastDataValues[u32_Index].size() > 1))
                           {
                              if (u32_Index < this->mc_Units.size())
                              {
                                 c_Retval =
                                    static_cast<QString>("%1 %2").arg(m_GetValue(u32_Index)).arg(mc_Units[u32_Index]);
                              }
                           }
                        }
                        break;
                     case C_SyvDaItTaModel::eBAR:
                     default:
                        break;
                     }
                  }
                  else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_TYPE)
                  {
                     QString c_Error;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        bool q_IsTransmissionError;
                        if ((pc_TableWidget->GetViewActive(*pc_DataElementId) == false) ||
                            (pc_TableWidget->GetViewDashboardRouteValid(*pc_DataElementId) == false))
                        {
                           c_Retval = static_cast<int32_t>(C_NagToolTip::eWARNING);
                        }
                        else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                q_IsTransmissionError) == true)
                        {
                           c_Retval = static_cast<int32_t>(C_NagToolTip::eERROR);
                        }
                        else
                        {
                           //No tool tip
                        }
                        break;
                     case C_SyvDaItTaModel::eNAME:
                     case C_SyvDaItTaModel::eVALUE:
                     case C_SyvDaItTaModel::eBAR:
                     default:
                        c_Retval = static_cast<int32_t>(C_NagToolTip::eDEFAULT);
                        break;
                     }
                  }
                  else
                  {
                     //Unhandled
                  }
               }
            }
            else
            {
               //Invalid
               if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = "";
                     break;
                  case C_SyvDaItTaModel::eNAME:
                     c_Retval = pc_DataElementId->GetInvalidNamePlaceholder();
                     break;
                  case C_SyvDaItTaModel::eVALUE:
                     c_Retval = "";
                     break;
                  case C_SyvDaItTaModel::eBAR:
                     c_Retval = "";
                     break;
                  default:
                     break;
                  }
               }
               else if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eBAR:
                     c_Retval = 0.0;
                     break;
                  case C_SyvDaItTaModel::eICON:
                  case C_SyvDaItTaModel::eNAME:
                  case C_SyvDaItTaModel::eVALUE:
                  default:
                     break;
                  }
               }
               else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = C_GtGetText::h_GetText("Configuration warning");
                     break;
                  case C_SyvDaItTaModel::eNAME:
                  case C_SyvDaItTaModel::eVALUE:
                  case C_SyvDaItTaModel::eBAR:
                  default:
                     break;
                  }
               }
               else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     c_Retval = pc_DataElementId->GetInvalidNamePlaceholder();
                     break;
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = pc_DataElementId->GetInvalidItemPlaceholderTooltip();
                     break;
                  case C_SyvDaItTaModel::eVALUE:
                  case C_SyvDaItTaModel::eBAR:
                  default:
                     break;
                  }
               }
               else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_TYPE)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     c_Retval = static_cast<int32_t>(C_NagToolTip::eDEFAULT);
                     break;
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = static_cast<int32_t>(C_NagToolTip::eWARNING);
                     break;
                  case C_SyvDaItTaModel::eVALUE:
                  case C_SyvDaItTaModel::eBAR:
                  default:
                     break;
                  }
               }
               else if (os32_Role == ms32_USER_ROLE_ICON)
               {
                  QStringList c_Icons;
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     break;
                  case C_SyvDaItTaModel::eICON:
                     c_Icons.push_back(QString::number(16));
                     switch (pc_DataElementId->GetInvalidTypePlaceholder())
                     {
                     // first string is normal icon, second string selected icon
                     //(no selected color specified here, so just use same icon)
                     case C_OscNodeDataPool::eDIAG:
                        c_Icons.append(this->mc_IconVariableWarning);
                        c_Retval = c_Icons;
                        break;
                     case C_OscNodeDataPool::eNVM:
                        c_Icons.append(this->mc_IconParameterWarning);
                        c_Retval = c_Icons;
                        break;
                     case C_OscNodeDataPool::eCOM:
                        c_Icons.append(this->mc_IconSignalWarning);
                        c_Retval = c_Icons;
                        break;
                     case C_OscNodeDataPool::eHALC:
                     case C_OscNodeDataPool::eHALC_NVM:
                        c_Icons.append(this->mc_IconHalOtherWarning); // in warning case no different HAL icons
                        c_Retval = c_Icons;
                        break;
                     default:
                        break;
                     }
                     break;
                  case eVALUE:
                  case eBAR:
                  default:
                     break;
                  }
               }
               else
               {
                  //Unhandled
               }
            }
         }
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle copy items action

   This is not yet implemented and needs to be implemented by inheritors,
   because a generic function would not make anything easier as any copy action depends on the data.

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)
*/
//-----------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_SyvDaItTaModel::CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const
{
   //Copy paste not supported
   Q_UNUSED(orc_SelectedIndices)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle move selected items action

   Warning 1: This function currently can only handle shifts by ONE up or down

   \param[in]  orc_Indices    Selected indices
   \param[in]  oq_Up          Flag to switch to move one step up or down
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::MoveItems(const QModelIndexList & orc_Indices, const bool oq_Up)
{
   this->MoveSelectedItems(C_SyvDaItTaModel::mh_GetSelectedRows(orc_Indices), oq_Up);
   //Reinitialize static content
   InitMinMaxAndName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new data element

   \param[in]  orc_Indices             Item(s) after that the new element should be added (usually only one item)
   \param[in]  orc_DataPoolElementId   New data element ID

   \return
   Index of new item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaItTaModel::AddItem(const QModelIndexList & orc_Indices,
                                   const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
{
   this->mc_AddDataPoolElementId = orc_DataPoolElementId;
   return this->AddNewItem(C_SyvDaItTaModel::mh_GetSelectedRows(orc_Indices));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove items

   \param[in]   orc_Indices               Items to remove
   \param[out]  orc_RemovedDataElements   Removed data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::RemoveItems(const QModelIndexList & orc_Indices,
                                   std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements)
{
   this->mc_RemovedDataPoolElementIds.clear();
   this->DeleteSelectedItems(C_SyvDaItTaModel::mh_GetSelectedRows(orc_Indices));
   orc_RemovedDataElements = this->mc_RemovedDataPoolElementIds;
   //Reinitialize static content
   InitMinMaxAndName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItTaModel::E_Columns C_SyvDaItTaModel::h_ColumnToEnum(const int32_t & ors32_Column)
{
   C_SyvDaItTaModel::E_Columns e_Retval = eICON;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eICON;
      break;
   case 1:
      e_Retval = eNAME;
      break;
   case 2:
      e_Retval = eVALUE;
      break;
   case 3:
      e_Retval = eBAR;
      break;
   default:
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in]  ore_Value   Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaItTaModel::h_EnumToColumn(const C_SyvDaItTaModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eICON:
      s32_Retval = 0;
      break;
   case eNAME:
      s32_Retval = 1;
      break;
   case eVALUE:
      s32_Retval = 2;
      break;
   case eBAR:
      s32_Retval = 3;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get ID for element index

   \param[in]  ou32_Index  Element index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId * C_SyvDaItTaModel::GetDataPoolElementIndex(const uint32_t ou32_Index) const
{
   const C_PuiSvDbNodeDataPoolListElementId * pc_Retval = NULL;

   if (this->mpc_Data != NULL)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_GiSvDaTableBase * const pc_Table = dynamic_cast<const C_GiSvDaTableBase *>(this->mpc_Data);
      if (pc_Table != NULL)
      {
         const C_PuiSvDbTable * const pc_TableData = pc_Table->GetTableItem();
         if ((pc_TableData != NULL) && (ou32_Index < pc_TableData->c_DataPoolElementsConfig.size()))
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_TableData->c_DataPoolElementsConfig[ou32_Index];
            pc_Retval = &rc_Config.c_ElementId;
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle add new item action

   \param[in] ou32_SelectedIndex Index to insert item at

   \return
   Index of new item
*/
//-----------------------------------------------------------------------------
uint32_t C_SyvDaItTaModel::m_AddNewItem(const uint32_t ou32_SelectedIndex)
{
   uint32_t u32_Retval = 0UL;

   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if ((pc_TableWidget != NULL) && (this->mc_AddDataPoolElementId.GetIsValid() == true))
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if ((pc_Item != NULL) && (pc_Item->c_DataPoolElementsConfig.size() < C_SyvDaItTaModel::hu32_MAX_ELEMENTS))
      {
         const C_PuiSvDbNodeDataElementConfig c_NewConfig = C_SyvDaItTaModel::mh_GetConfigForNewItem(
            this->mc_AddDataPoolElementId);
         C_PuiSvDbTable c_Copy = *pc_Item;
         bool q_ChangeRowCount;
         int32_t s32_InsertAt;

         //Check if new row
         if (pc_Item->c_DataPoolElementsConfig.size() > 0)
         {
            q_ChangeRowCount = true;
         }
         else
         {
            q_ChangeRowCount = false;
         }

         if (q_ChangeRowCount == true)
         {
            // get location where to insert new row
            // use valid index
            if (ou32_SelectedIndex < m_GetSizeItems())
            {
               s32_InsertAt = static_cast<int32_t>(ou32_SelectedIndex) + 1;
            }
            else
            {
               s32_InsertAt = m_GetSizeItems();
            }
            u32_Retval = static_cast<uint32_t>(s32_InsertAt);
         }
         else
         {
            //Always insert if start if first item
            s32_InsertAt = 0;
            u32_Retval = 0UL;
         }

         if (q_ChangeRowCount == true)
         {
            //New row
            beginInsertRows(QModelIndex(), s32_InsertAt, s32_InsertAt);
         }
         else
         {
            //Existing row changed
            beginResetModel();
         }
         //Add new item
         c_Copy.c_DataPoolElementsConfig.insert(c_Copy.c_DataPoolElementsConfig.begin() + s32_InsertAt, c_NewConfig);
         //Apply
         tgl_assert(pc_TableWidget->SetTableItem(c_Copy) == C_NO_ERR);
         //Register
         if ((this->mpc_Data != NULL) &&
             (this->mpc_Data->IsDataElementRegistered(this->mc_AddDataPoolElementId) == false))
         {
            tgl_assert(this->mpc_Data->RegisterDataPoolElement(this->mc_AddDataPoolElementId,
                                                               c_NewConfig.c_ElementScaling,
                                                               c_NewConfig.c_DisplayFormatter) == C_NO_ERR);
         }
         //Reinitialize static content
         m_AddAndInitMinMaxAndNameForItem(static_cast<uint32_t>(s32_InsertAt));
         if (q_ChangeRowCount == true)
         {
            //New row
            endInsertRows();
         }
         else
         {
            //Existing row changed
            endResetModel();
         }
      }
   }
   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle paste items action

   \param[in] ou32_SelectedIndex Index to insert item at

   \return
   Indices of new items
*/
//-----------------------------------------------------------------------------
std::vector<uint32_t> C_SyvDaItTaModel::m_PasteItems(const uint32_t ou32_SelectedIndex)
{
   std::vector<uint32_t> c_Retval;
   //Paste not supported
   Q_UNUSED(ou32_SelectedIndex)
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get size of item container, i.e. number of items.

   \return
   Size of item container
*/
//-----------------------------------------------------------------------------
uint32_t C_SyvDaItTaModel::m_GetSizeItems(void) const
{
   uint32_t u32_Retval = 0UL;

   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<const stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         u32_Retval = pc_Item->c_DataPoolElementsConfig.size();
      }
   }

   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete specific item

   Warning: not expected to fail

   \param[in] ou32_Index Index to delete
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::m_DeleteItem(const uint32_t ou32_Index)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if ((pc_Item != NULL) && (pc_Item->c_DataPoolElementsConfig.size() > 0))
      {
         C_PuiSvDbTable c_Copy = *pc_Item;
         //Step 2: Remove rows and store the according data element index
         const C_PuiSvDbNodeDataPoolListElementId c_Item = this->m_RemoveItem(ou32_Index,
                                                                              c_Copy.c_DataPoolElementsConfig);
         this->mc_RemovedDataPoolElementIds.push_back(c_Item);
         tgl_assert(pc_TableWidget->SetTableItem(c_Copy) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger table signal for start of remove action

   \param[in]  ou32_FirstIndex   Lowest index of this section of removed items
   \param[in]  ou32_LastIndex    Highest index of this section of removed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_BeginRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex)
{
   const uint32_t u32_DeletedItemCount = ou32_LastIndex - ou32_FirstIndex;

   if (this->m_GetSizeItems() == u32_DeletedItemCount)
   {
      this->beginResetModel();
   }
   else
   {
      C_TblModelAction::m_BeginRemoveRows(ou32_FirstIndex, ou32_LastIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger table signal for end of remove action

   \param[in]  ou32_FirstIndex   Lowest index of this section of removed items
   \param[in]  ou32_LastIndex    Highest index of this section of removed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_EndRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex)
{
   const uint32_t u32_DeletedItemCount = ou32_LastIndex - ou32_FirstIndex;

   if (this->m_GetSizeItems() == u32_DeletedItemCount)
   {
      this->endResetModel();
   }
   else
   {
      C_TblModelAction::m_EndRemoveRows(ou32_FirstIndex, ou32_LastIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move specific item

   For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
   Warning: not expected to fail

   \param[in] ou32_SourceIndex Source index
   \param[in] ou32_TargetIndex Target index
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if ((pc_Item != NULL) && (ou32_SourceIndex < pc_Item->c_DataPoolElementsConfig.size()))
      {
         const C_PuiSvDbNodeDataElementConfig c_NewConfig = pc_Item->c_DataPoolElementsConfig[ou32_SourceIndex];
         C_PuiSvDbTable c_Copy = *pc_Item;

         //Delete
         c_Copy.c_DataPoolElementsConfig.erase(c_Copy.c_DataPoolElementsConfig.begin() + ou32_SourceIndex);
         //Add new item
         c_Copy.c_DataPoolElementsConfig.insert(c_Copy.c_DataPoolElementsConfig.begin() + ou32_TargetIndex,
                                                c_NewConfig);
         //Apply
         tgl_assert(pc_TableWidget->SetTableItem(c_Copy) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get prepare configuration for new item

   \param[in]  orc_DataPoolElementId   New item ID

   \return
   Prepare configuration for new item
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataElementConfig C_SyvDaItTaModel::mh_GetConfigForNewItem(
   const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
{
   C_PuiSvDbNodeDataElementConfig c_NewConfig;
   const C_OscNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                 orc_DataPoolElementId.u32_DataPoolIndex,
                                                                 orc_DataPoolElementId.u32_ListIndex,
                                                                 orc_DataPoolElementId.u32_ElementIndex);

   c_NewConfig.c_ElementId = orc_DataPoolElementId;
   c_NewConfig.c_ElementScaling.q_UseDefault = true;
   if (pc_Element != NULL)
   {
      c_NewConfig.c_ElementScaling.f64_Offset = pc_Element->f64_Offset;
      c_NewConfig.c_ElementScaling.f64_Factor = pc_Element->f64_Factor;
      c_NewConfig.c_ElementScaling.c_Unit = pc_Element->c_Unit.c_str();
   }

   c_NewConfig.c_DisplayFormatter.q_IsActive = false;
   c_NewConfig.c_DisplayFormatter.c_FormatterString = "";

   return c_NewConfig;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete item in the provided array and unregister index if not used after this move

   \param[in]      ou32_Index          Index to delete
   \param[in,out]  orc_AdaptedItems    Items to change

   \return
   Deleted index
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId C_SyvDaItTaModel::m_RemoveItem(const uint32_t ou32_Index,
                                                                  std::vector<C_PuiSvDbNodeDataElementConfig> & orc_AdaptedItems)
{
   C_PuiSvDbNodeDataPoolListElementId c_Retval;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_CurId = this->GetDataPoolElementIndex(ou32_Index);

   if (pc_CurId != NULL)
   {
      bool q_Found = false;
      c_Retval = *pc_CurId;
      //Remove
      orc_AdaptedItems.erase(orc_AdaptedItems.begin() + ou32_Index);
      //Check other usages
      for (uint32_t u32_ItConfig = 0; u32_ItConfig < orc_AdaptedItems.size(); ++u32_ItConfig)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = orc_AdaptedItems[u32_ItConfig];
         if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementId == *pc_CurId))
         {
            q_Found = true;
         }
      }
      if ((q_Found == false) && (this->mpc_Data != NULL))
      {
         //Remove completely if no usages
         this->mpc_Data->RemoveDataPoolElement(*pc_CurId);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value in percent

   \param[in]  ou32_Index  Data element index

   \return
   Value in percent
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_SyvDaItTaModel::m_GetPercentage(const uint32_t ou32_Index) const
{
   float32_t f32_Retval = 0.0F;

   if (((((((((ou32_Index < this->mc_UnscaledLastDataValues.size()) &&
              (ou32_Index < this->mc_UnscaledMinValues.size())) &&
             (ou32_Index < this->mc_UnscaledMaxValues.size())) &&
            (ou32_Index < this->mc_ArrayItemIndex.size())) &&
           (this->mc_UnscaledLastDataValues[ou32_Index].size() > this->mc_ArrayItemIndex[ou32_Index])) &&
          (this->mc_UnscaledMinValues[ou32_Index].size() > this->mc_ArrayItemIndex[ou32_Index])) &&
         (this->mc_UnscaledMaxValues[ou32_Index].size() > this->mc_ArrayItemIndex[ou32_Index])) &&
        (ou32_Index < this->mc_ShowPercentage.size())) &&
       (this->mc_ShowPercentage[ou32_Index] == true))
   {
      const float64_t & rf64_Value = this->mc_UnscaledLastDataValues[ou32_Index][this->mc_ArrayItemIndex[ou32_Index]];
      const float64_t & rf64_Min = this->mc_UnscaledMinValues[ou32_Index][this->mc_ArrayItemIndex[ou32_Index]];
      const float64_t & rf64_Max = this->mc_UnscaledMaxValues[ou32_Index][this->mc_ArrayItemIndex[ou32_Index]];
      const float64_t f64_Range = rf64_Max - rf64_Min;
      if (f64_Range > 0.0)
      {
         //Convert
         f32_Retval = static_cast<float32_t>((rf64_Value - rf64_Min) / f64_Range);

         //Limit
         f32_Retval = std::max(f32_Retval, 0.0F);
         f32_Retval = std::min(f32_Retval, 1.0F);
      }
      else
      {
         //No range leads to 100%
         f32_Retval = 1.0F;
      }
   }

   return f32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get value encoded into QVariant

   \param[in]  ou32_Index  Data element index

   \return
   Value encoded into QVariant
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItTaModel::m_GetValue(const uint32_t ou32_Index) const
{
   QString c_Retval = "";

   if (((((this->mpc_Data != NULL) &&
          (ou32_Index < this->mc_UnscaledLastDataValues.size())) &&
         (ou32_Index < this->mc_InterpretAsStringFlags.size())) && (ou32_Index < this->mc_ShowPercentage.size())) &&
       (ou32_Index < this->mc_ArrayItemIndex.size()))
   {
      if (this->mc_InterpretAsStringFlags[ou32_Index] == false)
      {
         if (this->mc_UnscaledLastDataValues[ou32_Index].size() > 1)
         {
            if (this->mc_ShowPercentage[ou32_Index])
            {
               //Single value of array
               c_Retval = this->mc_ScaledDisplayDataValues[ou32_Index][this->
                                                                       mc_ArrayItemIndex
                                                                       [ou32_Index
                                                                       ]];
            }
            else
            {
               //Complete array
               for (uint32_t u32_It = 0; u32_It < this->mc_ScaledDisplayDataValues[ou32_Index].size(); ++u32_It)
               {
                  if (u32_It > 0)
                  {
                     c_Retval += ";";
                  }

                  c_Retval += this->mc_ScaledDisplayDataValues[ou32_Index][u32_It];
               }
            }
         }
         else if (this->mc_ScaledDisplayDataValues[ou32_Index].size() == 1)
         {
            //Single value
            c_Retval = this->mc_ScaledDisplayDataValues[ou32_Index][0UL];
         }
         else
         {
            // Nothing to do. No value in the vector.
         }
      }
      else
      {
         //String in first element
         tgl_assert(this->mc_ScaledDisplayDataValues[ou32_Index].size() >= 1);
         if (this->mc_ScaledDisplayDataValues[ou32_Index].size() >= 1)
         {
            c_Retval = this->mc_ScaledDisplayDataValues[ou32_Index][0UL];
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all row indices from the selected indices (unique)

   \param[in]  orc_Indices    Model indices

   \return
   Unique row indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_SyvDaItTaModel::mh_GetSelectedRows(const QModelIndexList & orc_Indices)
{
   std::vector<uint32_t> c_Retval;
   for (QModelIndexList::const_iterator c_It = orc_Indices.begin(); c_It != orc_Indices.end(); ++c_It)
   {
      c_Retval.push_back(c_It->row());
   }
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init min max and name for one row

   \param[in]  orc_ElementId        Element id
   \param[in]  orc_ElementConfig    Element config
   \param[in]  ou32_Index           Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_InitMinMaxAndNameForOneRow(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                                    const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig,
                                                    const uint32_t ou32_Index)
{
   if (orc_ElementId.GetIsValid() == true)
   {
      const C_OscNodeDataPoolListElement * const pc_OscElement =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_ElementId.u32_NodeIndex,
                                                                    orc_ElementId.u32_DataPoolIndex,
                                                                    orc_ElementId.u32_ListIndex,
                                                                    orc_ElementId.u32_ElementIndex);
      const C_OscNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
         orc_ElementId.u32_NodeIndex, orc_ElementId.u32_DataPoolIndex);

      tgl_assert(pc_OscElement != NULL);
      tgl_assert(pc_Datapool != NULL);
      if ((pc_OscElement != NULL) && (pc_Datapool != NULL))
      {
         this->m_InitValuesForOneRow(orc_ElementId, orc_ElementConfig, *pc_OscElement, ou32_Index);

         if (orc_ElementConfig.c_DisplayName.compare("") == 0)
         {
            if ((pc_Datapool->e_Type == C_OscNodeDataPool::eHALC) ||
                (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
            {
               this->mc_Names[ou32_Index] = C_PuiSvHandler::h_GetShortNamespace(orc_ElementId);
            }
            else
            {
               if (orc_ElementId.GetUseArrayElementIndex())
               {
                  this->mc_Names[ou32_Index] = static_cast<QString>("%1[%2]").
                                               arg(pc_OscElement->c_Name.c_str()).
                                               arg(orc_ElementId.GetArrayElementIndex());
               }
               else
               {
                  this->mc_Names[ou32_Index] = pc_OscElement->c_Name.c_str();
               }
            }
         }
         else
         {
            this->mc_Names[ou32_Index] = orc_ElementConfig.c_DisplayName;
         }
         if (orc_ElementConfig.c_ElementScaling.q_UseDefault == true)
         {
            this->mc_Units[ou32_Index] = pc_OscElement->c_Unit.c_str();
         }
         else
         {
            this->mc_Units[ou32_Index] = orc_ElementConfig.c_ElementScaling.c_Unit;
         }
      }
   }
   else
   {
      const std::vector<float64_t> c_Empty;
      //Fill up values with dummies
      this->mc_Names[ou32_Index] = orc_ElementId.GetInvalidNamePlaceholder();
      this->mc_Units[ou32_Index] = "";
      this->mc_ShowPercentage[ou32_Index] = false;
      this->mc_InterpretAsStringFlags[ou32_Index] = false;
      this->mc_UnscaledMaxValues[ou32_Index] = c_Empty;
      this->mc_UnscaledMinValues[ou32_Index] = c_Empty;
      this->mc_ScaledDisplayDataValues[ou32_Index] = std::vector<QString>();
      this->mc_UnscaledLastDataValues[ou32_Index] = c_Empty;
      this->mc_ArrayItemIndex[ou32_Index] = 0UL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init values for one row

   \param[in]  orc_ElementId        Element id
   \param[in]  orc_ElementConfig    Element config
   \param[in]  orc_OscElement       Osc element
   \param[in]  ou32_Index           Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_InitValuesForOneRow(const C_PuiSvDbNodeDataPoolListElementId & orc_ElementId,
                                             const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig,
                                             const C_OscNodeDataPoolListElement & orc_OscElement,
                                             const uint32_t ou32_Index)
{
   std::vector<float64_t> c_Values;
   C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_OscElement.c_MinValue, c_Values);
   this->mc_UnscaledMinValues[ou32_Index] = c_Values;
   C_SdNdeDpContentUtil::h_GetValuesAsFloat64(orc_OscElement.c_MaxValue, c_Values);
   this->mc_UnscaledMaxValues[ou32_Index] = c_Values;
   this->m_InitStartValueForOneRow(orc_ElementConfig, orc_OscElement, ou32_Index);
   this->mc_ArrayItemIndex[ou32_Index] = orc_ElementId.GetArrayElementIndexOrZero();

   //Percentage
   if (orc_OscElement.GetArray())
   {
      if (orc_ElementId.GetUseArrayElementIndex())
      {
         this->mc_ShowPercentage[ou32_Index] = true;
      }
      else
      {
         this->mc_ShowPercentage[ou32_Index] = false;
      }
   }
   else
   {
      this->mc_ShowPercentage[ou32_Index] = true;
   }

   //String
   this->mc_InterpretAsStringFlags[ou32_Index] = orc_OscElement.q_InterpretAsString;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init start value for one row

   \param[in]  orc_ElementConfig    Element config
   \param[in]  orc_OscElement       Osc element
   \param[in]  ou32_Index           Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_InitStartValueForOneRow(const C_PuiSvDbNodeDataElementConfig & orc_ElementConfig,
                                                 const C_OscNodeDataPoolListElement & orc_OscElement,
                                                 const uint32_t ou32_Index)
{
   C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_FullyUsefulAndTotallyNecessaryVariable;
   C_OscNodeDataPoolContent c_Val = orc_OscElement.c_MinValue;
   std::vector<float64_t> c_Values;
   std::vector<QString> c_Formatted;
   C_PuiSvDbDataElementDisplayFormatterConfig c_Formatter;
   //Formatter
   c_Formatter.SetDisplayFormatter(orc_ElementConfig.c_DisplayFormatter);
   c_Formatter.SetType(
      C_PuiSvDbDataElementDisplayFormatter::h_GetTypeCategory(
         orc_OscElement.c_MinValue,
         orc_ElementConfig.c_ElementScaling,
         orc_OscElement.q_InterpretAsString));
   //Set unscaled starting value to 0 if possible
   C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(orc_OscElement.c_MinValue,
                                                         orc_OscElement.c_MaxValue, c_Val,
                                                         e_FullyUsefulAndTotallyNecessaryVariable,
                                                         C_OscNodeDataPoolContentUtil::eTO_ZERO);
   c_Formatted = c_Formatter.GetValuesContentFormatted(c_Val,
                                                       orc_ElementConfig.c_ElementScaling,
                                                       c_Values);
   this->mc_ScaledDisplayDataValues[ou32_Index] = c_Formatted;
   this->mc_UnscaledLastDataValues[ou32_Index] = c_Values;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add and init min max and name for item

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_AddAndInitMinMaxAndNameForItem(const uint32_t ou32_Index)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw::opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw::opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         //Reserve
         this->mc_ScaledDisplayDataValues.insert(this->mc_ScaledDisplayDataValues.begin() + ou32_Index,
                                                 std::vector<QString>());
         this->mc_UnscaledLastDataValues.insert(this->mc_UnscaledLastDataValues.begin() + ou32_Index,
                                                std::vector<float64_t>());
         this->mc_UnscaledMaxValues.insert(this->mc_UnscaledMaxValues.begin() + ou32_Index, std::vector<float64_t>());
         this->mc_UnscaledMinValues.insert(this->mc_UnscaledMinValues.begin() + ou32_Index, std::vector<float64_t>());
         this->mc_ArrayItemIndex.insert(this->mc_ArrayItemIndex.begin() + ou32_Index, 0UL);
         this->mc_Names.insert(this->mc_Names.begin() + ou32_Index, "");
         this->mc_Units.insert(this->mc_Units.begin() + ou32_Index, "");
         this->mc_InterpretAsStringFlags.insert(this->mc_InterpretAsStringFlags.begin() + ou32_Index, false);
         this->mc_ShowPercentage.insert(this->mc_ShowPercentage.begin() + ou32_Index, false);

         //Init
         this->mc_Transparency.insert(this->mc_Transparency.begin() + ou32_Index, 255);

         //Look up
         m_InitMinMaxAndNameForItem(*pc_Item, ou32_Index);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init min max and name for item

   \param[in]  orc_Table   Table
   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaModel::m_InitMinMaxAndNameForItem(const C_PuiSvDbTable & orc_Table, const uint32_t ou32_Index)
{
   const C_PuiSvDbNodeDataPoolListElementId * const pc_ElementId =
      this->GetDataPoolElementIndex(ou32_Index);

   if (pc_ElementId != NULL)
   {
      this->m_InitMinMaxAndNameForOneRow(*pc_ElementId,
                                         orc_Table.c_DataPoolElementsConfig[ou32_Index], ou32_Index);
   }
}
