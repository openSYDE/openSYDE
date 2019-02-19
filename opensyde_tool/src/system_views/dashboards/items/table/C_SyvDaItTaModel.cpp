//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Table model for dashboard table widget (implementation)

   Table model for dashboard table widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "C_Uti.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_SyvUtil.h"
#include "C_GtGetText.h"
#include "C_NagToolTip.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvDaTableBase.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_SyvDaItTaModel.h"
#include "C_GiSvDaTableBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
//Cast to integer on some level so this is the allowed maximum
const uint32 C_SyvDaItTaModel::hu32_MaxElements = static_cast<uint32>(std::numeric_limits<sintn>::max());

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Data   Data storage
   \param[in,out] opc_Parent Optional pointer to parent

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItTaModel::C_SyvDaItTaModel(C_PuiSvDbDataElementHandler * const opc_Data, QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mpc_Data(opc_Data)
{
   //Default icons
   this->mc_IconParameter = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconParameter.svg");
   this->mc_IconSignal = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconSignal.svg");
   this->mc_IconVariable = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconVariable.svg");

   //Warning icons
   this->mc_IconParameterWarning = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconParameterWarning.svg");
   this->mc_IconVariableWarning = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconVariableWarning.svg");
   this->mc_IconSignalWarning = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconSignalWarning.svg");

   //Error icons
   this->mc_IconParameterError = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconParameterError.svg");
   this->mc_IconVariableError = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconVariableError.svg");
   this->mc_IconSignalError = C_SdUtil::h_InitStaticIcon(
      "://images/system_definition/IconSignalError.svg");

   //Register to allow data changed signal
   qRegisterMetaType<QVector<sintn> >();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active

   \created     05.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   switch (oe_Style)
   {
   //TBD by Karsten: done
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
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize min & max values and names for data elements

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::InitMinMaxAndName(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         const uint32 u32_Count = pc_Item->c_DataPoolElementsConfig.size();

         //Clear
         this->mc_UnscaledLastDataValues.clear();
         this->mc_UnscaledMaxValues.clear();
         this->mc_UnscaledMinValues.clear();
         this->mc_Names.clear();
         this->mc_Units.clear();
         this->mc_Transparency.clear();
         this->mc_InterpretAsStringFlags.clear();
         this->mc_ShowPercentage.clear();

         //Reserve
         this->mc_UnscaledLastDataValues.reserve(u32_Count);
         this->mc_UnscaledMaxValues.reserve(u32_Count);
         this->mc_UnscaledMinValues.reserve(u32_Count);
         this->mc_Names.reserve(u32_Count);
         this->mc_Units.reserve(u32_Count);
         this->mc_InterpretAsStringFlags.reserve(u32_Count);
         this->mc_ShowPercentage.reserve(u32_Count);

         //Init
         this->mc_Transparency.resize(u32_Count, 255);

         //Look up
         for (uint32 u32_ItElement = 0; u32_ItElement < u32_Count; ++u32_ItElement)
         {
            const C_PuiSvDbNodeDataPoolListElementId * const pc_ElementId =
               this->GetDataPoolElementIndex(u32_ItElement);
            if (pc_ElementId != NULL)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Item->c_DataPoolElementsConfig[u32_ItElement];
               if (pc_ElementId->GetIsValid() == true)
               {
                  const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(pc_ElementId->u32_NodeIndex,
                                                                                pc_ElementId->u32_DataPoolIndex,
                                                                                pc_ElementId->u32_ListIndex,
                                                                                pc_ElementId->u32_ElementIndex);
                  const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
                     C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(pc_ElementId->u32_NodeIndex,
                                                                               pc_ElementId->u32_DataPoolIndex,
                                                                               pc_ElementId->u32_ListIndex,
                                                                               pc_ElementId->u32_ElementIndex);
                  tgl_assert(pc_OSCElement != NULL);
                  if (pc_OSCElement != NULL)
                  {
                     C_SdNdeDataPoolContentUtil::E_ValueChangedTo e_FullyUsefulAndTotallyNecessaryVariable;
                     C_OSCNodeDataPoolContent c_Val = pc_OSCElement->c_MinValue;
                     std::vector<float64> c_Values;
                     C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(pc_OSCElement->c_MinValue, c_Values);
                     this->mc_UnscaledMinValues.push_back(c_Values);
                     //Set unscaled starting value to 0 if possible
                     C_SdNdeDataPoolContentUtil::h_SetValueInMinMaxRange(pc_OSCElement->c_MinValue,
                                                                         pc_OSCElement->c_MaxValue, c_Val,
                                                                         e_FullyUsefulAndTotallyNecessaryVariable,
                                                                         C_SdNdeDataPoolContentUtil::eTO_ZERO);
                     C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(c_Val, c_Values);
                     this->mc_UnscaledLastDataValues.push_back(c_Values);
                     C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(pc_OSCElement->c_MaxValue, c_Values);
                     this->mc_UnscaledMaxValues.push_back(c_Values);
                     if (rc_Config.c_DisplayName.compare("") == 0)
                     {
                        this->mc_Names.push_back(pc_OSCElement->c_Name.c_str());
                     }
                     else
                     {
                        this->mc_Names.push_back(rc_Config.c_DisplayName);
                     }
                     if (rc_Config.c_ElementScaling.q_UseDefault == true)
                     {
                        this->mc_Units.push_back(pc_OSCElement->c_Unit.c_str());
                     }
                     else
                     {
                        this->mc_Units.push_back(rc_Config.c_ElementScaling.c_Unit);
                     }
                     //Percentage
                     if (pc_OSCElement->GetArray() == true)
                     {
                        this->mc_ShowPercentage.push_back(false);
                     }
                     else
                     {
                        this->mc_ShowPercentage.push_back(true);
                     }
                  }
                  //String
                  tgl_assert(pc_UIElement != NULL);
                  if (pc_UIElement != NULL)
                  {
                     this->mc_InterpretAsStringFlags.push_back(pc_UIElement->q_InterpretAsString);
                  }
               }
               else
               {
                  std::vector<float64> c_Empty;
                  //Fill up values with dummies
                  this->mc_Names.push_back(pc_ElementId->GetInvalidNamePlaceholder());
                  this->mc_Units.push_back("");
                  this->mc_ShowPercentage.push_back(false);
                  this->mc_InterpretAsStringFlags.push_back(false);
                  this->mc_UnscaledMaxValues.push_back(c_Empty);
                  this->mc_UnscaledMinValues.push_back(c_Empty);
                  this->mc_UnscaledLastDataValues.push_back(c_Empty);
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get newest values

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateValue(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         const uint32 u32_Count = pc_TableWidget->GetWidgetDataPoolElementCount();

         //For each unique ID
         for (uint32 u32_ItElement = 0; u32_ItElement < u32_Count; ++u32_ItElement)
         {
            C_PuiSvDbNodeDataPoolListElementId c_Id;
            if (pc_TableWidget->GetDataPoolElementIndex(u32_ItElement, c_Id) == C_NO_ERR)
            {
               std::vector<float64> c_UnscaledValues;
               if ((pc_TableWidget->GetLastValueUnscaled(u32_ItElement, c_UnscaledValues) == C_NO_ERR) &&
                   (c_UnscaledValues.size() > 0))
               {
                  //For each table item
                  for (uint32 u32_ItConfig = 0; u32_ItConfig < pc_Item->c_DataPoolElementsConfig.size();
                       ++u32_ItConfig)
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        pc_Item->c_DataPoolElementsConfig[u32_ItConfig];
                     if (rc_Config.c_ElementId == c_Id)
                     {
                        //Set unscaled value
                        if (u32_ItConfig < this->mc_UnscaledLastDataValues.size())
                        {
                           this->mc_UnscaledLastDataValues[u32_ItConfig] = c_UnscaledValues;
                        }
                        else
                        {
                           this->mc_UnscaledLastDataValues.resize(u32_ItConfig + 1);
                           this->mc_UnscaledLastDataValues[u32_ItConfig] = c_UnscaledValues;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update the error icon

   \created     09.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateError(void)
{
   if (this->rowCount() > 0)
   {
      QVector<sintn> c_Roles;
      c_Roles.push_back(static_cast<sintn>(Qt::DecorationRole));
      Q_EMIT dataChanged(this->index(0, 0), this->index(this->rowCount() - 1, 0), c_Roles);
      //QVector<sintn>() << static_cast<sintn>(Qt::DecorationRole));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update of the color transparence value configured by the actual timeout state

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] osn_Value                       Value for transparence (0..255)

   \created     05.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (pc_Item != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id;
         if (pc_TableWidget->GetDataPoolElementIndex(ou32_DataElementIndex, c_Id) == C_NO_ERR)
         {
            for (uint32 u32_ItConfig = 0; u32_ItConfig < pc_Item->c_DataPoolElementsConfig.size(); ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Item->c_DataPoolElementsConfig[u32_ItConfig];
               if ((rc_Config.c_ElementId == c_Id) && (u32_ItConfig < this->mc_Transparency.size()))
               {
                  const QModelIndex c_Index =
                     this->index(static_cast<sintn>(u32_ItConfig), h_EnumToColumn(C_SyvDaItTaModel::eVALUE));
                  this->mc_Transparency[u32_ItConfig] = osn_Value;
                  //lint -e{1793} Qt example
                  Q_EMIT dataChanged(c_Index, c_Index, QVector<sintn>() << static_cast<sintn>(Qt::ForegroundRole));
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get item indices

   \param[in] orc_Indices     Model indices
   \param[in] orc_ItemIndices Item indices

   \created     10.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::GetUniqueRows(const QModelIndexList & orc_Indices, std::vector<uint32> & orc_ItemIndices) const
{
   //Step 1: extract rows
   orc_ItemIndices.reserve(orc_Indices.count());
   for (QModelIndexList::const_iterator c_ItIndex = orc_Indices.begin(); c_ItIndex != orc_Indices.end(); ++c_ItIndex)
   {
      if (c_ItIndex->row() >= 0)
      {
         orc_ItemIndices.push_back(static_cast<uint32>(c_ItIndex->row()));
      }
   }
   if (orc_ItemIndices.size() > 0)
   {
      //Step 2: Sort rows descending (otherwise the indices must be changed)
      orc_ItemIndices = C_Uti::h_UniquifyAndSortDescending(orc_ItemIndices);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get header data

   \param[in] osn_Section    Section
   \param[in] oe_Orientation Orientation
   \param[in] osn_Role       Role

   \return
   Header string

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SyvDaItTaModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                      const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SyvDaItTaModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         switch (e_Col)
         {
         case eVALUE:
            c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
            break;
         case eBAR:
            c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
            break;
         default:
            c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get table row count

   \param[in] orc_Parent Parent

   \return
   Row count

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaItTaModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
         dynamic_cast<const stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

      if (pc_TableWidget != NULL)
      {
         const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
         if (pc_Item != NULL)
         {
            if (pc_Item->c_DataPoolElementsConfig.size() == 0)
            {
               sn_Retval = 1;
            }
            else
            {
               sn_Retval = pc_Item->c_DataPoolElementsConfig.size();
            }
         }
      }
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaItTaModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      sn_Retval = 4;
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get data at index

   \param[in] orc_Index Index
   \param[in] osn_Role  Data role

   \return
   Data

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SyvDaItTaModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if ((orc_Index.isValid()) && (orc_Index.row() >= 0))
   {
      const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
      const C_SyvDaItTaModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      const C_PuiSvDbNodeDataPoolListElementId * const pc_DataElementId = this->GetDataPoolElementIndex(u32_Index);
      if (pc_DataElementId != NULL)
      {
         //Generic settings, independent of invalid flag
         if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
         {
            switch (e_Col)
            {
            case eNAME:
               c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
               break;
            case eVALUE:
               c_Retval = QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
               break;
            default:
               break;
            }
         }
         else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
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
               const stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
                  dynamic_cast<const stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

               if (pc_TableWidget != NULL)
               {
                  //Default
                  if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
                           c_Retval = QString("%1 %2").arg(GetValue(u32_Index)).arg(mc_Units[u32_Index]);
                        }
                        break;
                     case C_SyvDaItTaModel::eBAR:
                        c_Retval = "";
                        break;
                     default:
                        break;
                     }
                  }
                  else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
                  {
                     const C_OSCNodeDataPool * pc_DataPool;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        pc_DataPool =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(pc_DataElementId->u32_NodeIndex,
                                                                           pc_DataElementId->u32_DataPoolIndex);
                        if (pc_DataPool != NULL)
                        {
                           bool q_IsTransmissionError;
                           QString c_Error;
                           if (pc_TableWidget->GetViewActive(*pc_DataElementId) == false)
                           {
                              switch (pc_DataPool->e_Type)
                              {
                              case C_OSCNodeDataPool::eDIAG:
                                 c_Retval = this->mc_IconVariableWarning;
                                 break;
                              case C_OSCNodeDataPool::eNVM:
                                 c_Retval = this->mc_IconParameterWarning;
                                 break;
                              case C_OSCNodeDataPool::eCOM:
                                 c_Retval = this->mc_IconSignalWarning;
                                 break;
                              default:
                                 break;
                              }
                           }
                           else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                   q_IsTransmissionError) == true)
                           {
                              switch (pc_DataPool->e_Type)
                              {
                              case C_OSCNodeDataPool::eDIAG:
                                 c_Retval = this->mc_IconVariableError;
                                 break;
                              case C_OSCNodeDataPool::eNVM:
                                 c_Retval = this->mc_IconParameterError;
                                 break;
                              case C_OSCNodeDataPool::eCOM:
                                 c_Retval = this->mc_IconSignalError;
                                 break;
                              default:
                                 break;
                              }
                           }
                           else
                           {
                              switch (pc_DataPool->e_Type)
                              {
                              case C_OSCNodeDataPool::eDIAG:
                                 c_Retval = this->mc_IconVariable;
                                 break;
                              case C_OSCNodeDataPool::eNVM:
                                 c_Retval = this->mc_IconParameter;
                                 break;
                              case C_OSCNodeDataPool::eCOM:
                                 c_Retval = this->mc_IconSignal;
                                 break;
                              default:
                                 break;
                              }
                           }
                        }
                        break;
                     default:
                        break;
                     }
                  }
                  else if (osn_Role == static_cast<sintn>(Qt::EditRole))
                  {
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eBAR:
                        c_Retval = this->GetPercentage(u32_Index);
                        break;
                     default:
                        break;
                     }
                  }
                  else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
                  {
                     bool q_IsTransmissionError;
                     QString c_Error;

                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        if (pc_TableWidget->GetViewActive(*pc_DataElementId) == false)
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
                        if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(*pc_DataElementId) != NULL)
                        {
                           c_Retval =
                              C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(*pc_DataElementId)->c_Name.
                              c_str();
                        }
                        break;
                     default:
                        break;
                     }
                  }
                  else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
                  {
                     bool q_IsTransmissionError;
                     QString c_Error;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
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
                                 c_Retval = QString("%1 %2").arg(GetValue(u32_Index)).arg(mc_Units[u32_Index]);
                              }
                           }
                        }
                        break;
                     default:
                        break;
                     }
                  }
                  else if (osn_Role == msn_USER_ROLE_TOOL_TIP_TYPE)
                  {
                     bool q_IsTransmissionError;
                     QString c_Error;
                     switch (e_Col)
                     {
                     case C_SyvDaItTaModel::eICON:
                        if (pc_TableWidget->GetViewActive(*pc_DataElementId) == false)
                        {
                           c_Retval = static_cast<sintn>(C_NagToolTip::eWARNING);
                        }
                        else if (pc_TableWidget->CheckItemError(*pc_DataElementId, c_Error,
                                                                q_IsTransmissionError) == true)
                        {
                           c_Retval = static_cast<sintn>(C_NagToolTip::eERROR);
                        }
                        else
                        {
                           //No tool tip
                        }
                        break;
                     default:
                        c_Retval = static_cast<sintn>(C_NagToolTip::eDEFAULT);
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
               if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
               else if (osn_Role == static_cast<sintn>(Qt::EditRole))
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eBAR:
                     c_Retval = 0.0;
                     break;
                  default:
                     break;
                  }
               }
               else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = C_GtGetText::h_GetText("Configuration warning");
                     break;
                  default:
                     break;
                  }
               }
               else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     c_Retval = pc_DataElementId->GetInvalidNamePlaceholder();
                     break;
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = C_GtGetText::h_GetText("Data element was deleted in SYSTEM DEFINITION");
                     break;
                  default:
                     break;
                  }
               }
               else if (osn_Role == msn_USER_ROLE_TOOL_TIP_TYPE)
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     c_Retval = static_cast<sintn>(C_NagToolTip::eDEFAULT);
                     break;
                  case C_SyvDaItTaModel::eICON:
                     c_Retval = static_cast<sintn>(C_NagToolTip::eWARNING);
                     break;
                  default:
                     break;
                  }
               }
               else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
               {
                  switch (e_Col)
                  {
                  case C_SyvDaItTaModel::eNAME:
                     break;
                  case C_SyvDaItTaModel::eICON:
                     switch (pc_DataElementId->GetInvalidTypePlaceholder())
                     {
                     case C_OSCNodeDataPool::eDIAG:
                        c_Retval = this->mc_IconVariableWarning;
                        break;
                     case C_OSCNodeDataPool::eNVM:
                        c_Retval = this->mc_IconParameterWarning;
                        break;
                     case C_OSCNodeDataPool::eCOM:
                        c_Retval = this->mc_IconSignalWarning;
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
   \brief   Add new data element

   \param[in] orc_Indices           Item(s) after that the new element should be added (usually only one item)
   \param[in] orc_DataPoolElementId New data element ID

   \created     21.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::AddItem(const QModelIndexList & orc_Indices,
                               const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if ((pc_TableWidget != NULL) && (orc_DataPoolElementId.GetIsValid() == true))
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if (((pc_Item != NULL) && (pc_Item->c_DataPoolElementsConfig.size() < C_SyvDaItTaModel::hu32_MaxElements)) &&
          (pc_Element != NULL))
      {
         C_PuiSvDbNodeDataElementConfig c_NewConfig;
         C_PuiSvDbTable c_Copy = *pc_Item;
         bool q_ChangeRowCount;
         sint32 s32_InsertAt;

         //Check if new row
         if (pc_Item->c_DataPoolElementsConfig.size() > 0)
         {
            q_ChangeRowCount = true;
         }
         else
         {
            q_ChangeRowCount = false;
         }

         // get location where to insert new row (after selected vs. at the end)
         if ((orc_Indices.isEmpty() == false) && (q_ChangeRowCount == true))
         {
            // insert after (+1) last selected row (last())
            s32_InsertAt = orc_Indices.last().row() + 1;
         }
         else
         {
            // insert at the end
            s32_InsertAt = pc_Item->c_DataPoolElementsConfig.size();
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
         c_NewConfig.c_ElementId = orc_DataPoolElementId;
         c_NewConfig.c_ElementScaling.q_UseDefault = true;
         c_NewConfig.c_ElementScaling.f64_Offset = pc_Element->f64_Offset;
         c_NewConfig.c_ElementScaling.f64_Factor = pc_Element->f64_Factor;
         c_NewConfig.c_ElementScaling.c_Unit = pc_Element->c_Unit.c_str();
         //Apply
         c_Copy.c_DataPoolElementsConfig.insert(c_Copy.c_DataPoolElementsConfig.begin() + s32_InsertAt, c_NewConfig);
         tgl_assert(pc_TableWidget->SetTableItem(c_Copy) == C_NO_ERR);
         //Register
         if (this->mpc_Data->IsDataElementRegistered(orc_DataPoolElementId) == false)
         {
            tgl_assert(this->mpc_Data->RegisterDataPoolElement(orc_DataPoolElementId,
                                                               c_NewConfig.c_ElementScaling) == C_NO_ERR);
         }
         //Reinitialize static content
         InitMinMaxAndName();
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove items

   \param[in]  orc_Indices             Items to remove
   \param[out] orc_RemovedDataElements Removed data elements

   \created     21.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaModel::RemoveItems(const QModelIndexList & orc_Indices,
                                   std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   stw_opensyde_gui::C_GiSvDaTableBase * const pc_TableWidget =
      dynamic_cast<stw_opensyde_gui::C_GiSvDaTableBase * const>(this->mpc_Data);

   if (pc_TableWidget != NULL)
   {
      const C_PuiSvDbTable * const pc_Item = pc_TableWidget->GetTableItem();
      if ((pc_Item != NULL) && (pc_Item->c_DataPoolElementsConfig.size() > 0))
      {
         std::vector<uint32> c_TableRows;
         //Step 1: extract rows
         this->GetUniqueRows(orc_Indices, c_TableRows);
         if (c_TableRows.size() > 0)
         {
            std::vector<uint32> c_DataRows = c_TableRows;
            C_PuiSvDbTable c_Copy = *pc_Item;
            this->beginResetModel();
            //Step 2: Remove rows and store the according data element
            orc_RemovedDataElements.reserve(c_TableRows.size());
            for (uint32 u32_ItRow = 0; u32_ItRow < c_TableRows.size(); ++u32_ItRow)
            {
               const C_PuiSvDbNodeDataPoolListElementId * const pc_CurId =
                  this->GetDataPoolElementIndex(c_TableRows[u32_ItRow]);
               if (pc_CurId != NULL)
               {
                  bool q_Found = false;
                  //Adapt indices to deleted one (only necessary for the next ones)
                  for (uint32 u32_ItDataIndex = u32_ItRow; u32_ItDataIndex < c_DataRows.size(); ++u32_ItDataIndex)
                  {
                     uint32 & ru32_CurElement = c_DataRows[u32_ItDataIndex];
                     //Do NOT include the to be deleted one
                     if (ru32_CurElement > c_DataRows[u32_ItRow])
                     {
                        ru32_CurElement = ru32_CurElement - 1UL;
                     }
                  }
                  //Remove
                  c_Copy.c_DataPoolElementsConfig.erase(c_Copy.c_DataPoolElementsConfig.begin() +
                                                        c_DataRows[u32_ItRow]);
                  orc_RemovedDataElements.push_back(*pc_CurId);
                  //Check other usages
                  for (uint32 u32_ItConfig = 0; u32_ItConfig < c_Copy.c_DataPoolElementsConfig.size(); ++u32_ItConfig)
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config = c_Copy.c_DataPoolElementsConfig[u32_ItConfig];
                     if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementId == *pc_CurId))
                     {
                        q_Found = true;
                     }
                  }
                  if (q_Found == false)
                  {
                     //Remove completely if no usages
                     this->mpc_Data->RemoveDataPoolElement(*pc_CurId);
                  }
               }
            }
            tgl_assert(pc_TableWidget->SetTableItem(c_Copy) == C_NO_ERR);
            //Reinitialize static content
            InitMinMaxAndName();
            this->endResetModel();
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Column to enum conversion

   \param[in]  ors32_Column     Column
   \param[out] opc_DataSetIndex Optional: If column is a data set, this contains the data set index
                                          Else -1

   \return
   Enum value

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItTaModel::E_Columns C_SyvDaItTaModel::h_ColumnToEnum(const sint32 & ors32_Column)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Enum to column conversion

   \param[in] ore_Value Enum value

   \return
   Column
   -1 Error

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDaItTaModel::h_EnumToColumn(const C_SyvDaItTaModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Get ID for element index

   \param[in]  ou32_Index Element index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     11.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId * C_SyvDaItTaModel::GetDataPoolElementIndex(const uint32 ou32_Index) const
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
   \brief   Get value in percent

   \param[in] ou32_Index Data element index

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
float32 C_SyvDaItTaModel::GetPercentage(const uint32 ou32_Index) const
{
   float32 f32_Retval = 0.0F;

   if (((((((ou32_Index < this->mc_UnscaledLastDataValues.size()) &&
            (ou32_Index < this->mc_UnscaledMinValues.size())) &&
           (ou32_Index < this->mc_UnscaledMaxValues.size()) &&
           (this->mc_UnscaledLastDataValues[ou32_Index].size() > 0)) &&
          (this->mc_UnscaledMinValues[ou32_Index].size() > 0)) &&
         (this->mc_UnscaledMaxValues[ou32_Index].size() > 0)) && (ou32_Index < this->mc_ShowPercentage.size())) &&
       (this->mc_ShowPercentage[ou32_Index] == true))
   {
      const float64 & rf64_Value = this->mc_UnscaledLastDataValues[ou32_Index][0];
      const float64 & rf64_Min = this->mc_UnscaledMinValues[ou32_Index][0];
      const float64 & rf64_Max = this->mc_UnscaledMaxValues[ou32_Index][0];
      const float64 f64_Range = rf64_Max - rf64_Min;
      if (f64_Range > 0.0)
      {
         //Convert
         f32_Retval = static_cast<float32>((rf64_Value - rf64_Min) / f64_Range);

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

//-----------------------------------------------------------------------------
/*!
   \brief   Get value encoded into QVariant

   \param[in] ou32_Index Data element index

   \created     01.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvDaItTaModel::GetValue(const uint32 ou32_Index) const
{
   QString c_Retval = "";

   if ((this->mpc_Data != NULL) &&
       ((ou32_Index < this->mc_UnscaledLastDataValues.size()) && (ou32_Index < this->mc_InterpretAsStringFlags.size())))
   {
      if (this->mc_InterpretAsStringFlags[ou32_Index] == false)
      {
         if (this->mc_UnscaledLastDataValues[ou32_Index].size() > 1)
         {
            for (uint32 u32_It = 0; u32_It < this->mc_UnscaledLastDataValues[ou32_Index].size(); ++u32_It)
            {
               if (u32_It > 0)
               {
                  c_Retval += ";";
               }

               c_Retval +=
                  this->mpc_Data->GetUnscaledValueAsScaledString(this->mc_UnscaledLastDataValues[ou32_Index][u32_It],
                                                                 ou32_Index);
            }
         }
         else if (this->mc_UnscaledLastDataValues[ou32_Index].size() == 1)
         {
            c_Retval = this->mpc_Data->GetUnscaledValueAsScaledString(this->mc_UnscaledLastDataValues[ou32_Index][0UL],
                                                                      ou32_Index);
         }
         else
         {
            // Nothing to do. No value in the vector.
         }
      }
      else
      {
         std::vector<sint8> c_VecValues;
         uint32 u32_Counter;

         // Copy and cast the float values
         c_VecValues.resize(this->mc_UnscaledLastDataValues[ou32_Index].size());
         for (u32_Counter = 0U; u32_Counter < this->mc_UnscaledLastDataValues[ou32_Index].size(); ++u32_Counter)
         {
            c_VecValues[u32_Counter] = static_cast<sint8>(this->mc_UnscaledLastDataValues[ou32_Index][u32_Counter]);
         }

         c_Retval = C_SdNdeDataPoolUtil::h_ConvertToString(c_VecValues);
      }
   }

   return c_Retval;
}
