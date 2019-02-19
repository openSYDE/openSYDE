//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Table model for system view dashboard data element update mode (implementation)

   Table model for system view dashboard data element update mode

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "C_SdUtil.h"
#include "TGLUtils.h"
#include "constants.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_SyvDaPeUpdateModeTableModel.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \param[in]     ou32_ViewIndex View index
   \param[in]     ou32_NodeIndex Node index
   \param[in,out] opc_Parent     Optional pointer to parent

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableModel::C_SyvDaPeUpdateModeTableModel(const uint32 ou32_ViewIndex, const uint32 ou32_NodeIndex,
                                                             QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_ViewIndex(ou32_ViewIndex)
{
   //Create data
   m_Init(ou32_NodeIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply data from internal storage to global storage

   \created     12.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableModel::ApplyData(void) const
{
   tgl_assert(this->mc_UniqueDataElementIds.size() == this->mc_DataElementConfigurations.size());
   if (this->mc_UniqueDataElementIds.size() == this->mc_DataElementConfigurations.size())
   {
      for (uint32 u32_It = 0; u32_It < this->mc_UniqueDataElementIds.size(); ++u32_It)
      {
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetViewReadRailAssignment(this->mu32_ViewIndex,
                                                                               this->mc_UniqueDataElementIds[u32_It],
                                                                               this->mc_DataElementConfigurations[
                                                                                  u32_It]) == C_NO_ERR);
      }
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

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SyvDaPeUpdateModeTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                                   const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = "";
            break;
         case eINDEX:
            c_Retval = "#";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Data Element");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Value type");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Array size");
            break;
         case eDATA_POOL:
            c_Retval = C_GtGetText::h_GetText("Datapool");
            break;
         case eTRANSMISSION_MODE:
            c_Retval = C_GtGetText::h_GetText("Transmission Mode");
            break;
         case eCYCLIC_INTERVAL:
            c_Retval = C_GtGetText::h_GetText("Cyclic Interval");
            break;
         case eTHRESHOLD:
            c_Retval = C_GtGetText::h_GetText("Change threshold");
            break;
         case eUSAGE:
            c_Retval = C_GtGetText::h_GetText("Usage");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = "";
            break;
         case eINDEX:
            c_Retval = "#";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Data Element");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Value type");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Array size");
            break;
         case eDATA_POOL:
            c_Retval = C_GtGetText::h_GetText("Datapool");
            break;
         case eTRANSMISSION_MODE:
            c_Retval = C_GtGetText::h_GetText("Transmission Mode");
            break;
         case eCYCLIC_INTERVAL:
            c_Retval = C_GtGetText::h_GetText("Cyclic Interval");
            break;
         case eTHRESHOLD:
            c_Retval = C_GtGetText::h_GetText("Change threshold");
            break;
         case eUSAGE:
            c_Retval = C_GtGetText::h_GetText("Usage");
            break;
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = C_GtGetText::h_GetText("Type of data element");
            break;
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Item number");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Data element namespace from SYSTEM DEFINITION");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Data element value type from SYSTEM DEFINITION");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Data element array size from SYSTEM DEFINITION");
            break;
         case eDATA_POOL:
            c_Retval = C_GtGetText::h_GetText("Parent Datapool name from SYSTEM DEFINITION");
            break;
         case eTRANSMISSION_MODE:
            c_Retval = C_GtGetText::h_GetText("Used transmission mode:\n"
                                              "\"Cyclic\": transmit in fix time interval\n"
                                              "\"On Change\": transmit in fix time interval or earlier if change detected\n"
                                              "\"On Trigger\": Transmission is initiated by device application");
            break;
         case eCYCLIC_INTERVAL:
            c_Retval = C_GtGetText::h_GetText(
               "Fix time interval for transmission.\n"
               "Only necessary if transmission mode is \"On Change\" or \"Cyclic\"");
            break;
         case eTHRESHOLD:
            c_Retval = C_GtGetText::h_GetText("Threshold for detected change to trigger a value update.\n"
                                              "Only necessary if transmission mode is \"On Change\"");
            break;
         case eUSAGE:
            c_Retval = C_GtGetText::h_GetText("Number of items on each Dashboard which uses this data element");
            break;
         default:
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

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaPeUpdateModeTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //Top level
      return this->mc_UniqueDataElementIds.size();
   }

   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get table column count

   \param[in] orc_Parent Parent

   \return
   Column count

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaPeUpdateModeTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //Top level
      sn_Retval = 10;
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

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QVariant C_SyvDaPeUpdateModeTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      if (orc_Index.row() >= 0)
      {
         const uint32 u32_Index = orc_Index.row();
         if ((u32_Index < this->mc_UniqueDataElementIds.size()) &&
             (u32_Index < this->mc_DataElementConfigurations.size()))
         {
            const C_OSCNodeDataPoolListElementId & rc_CurId = this->mc_UniqueDataElementIds[u32_Index];
            const C_PuiSvReadDataConfiguration & rc_CurConfig = this->mc_DataElementConfigurations[u32_Index];
            const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
            if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
            {
               const C_OSCNodeDataPool * pc_DataPool;
               const C_OSCNodeDataPoolListElement * pc_OSCElement;
               const C_PuiSdNodeDataPoolListElement * pc_UIElement;
               switch (e_Col)
               {
               case eICON:
                  //No content
                  break;
               case eINDEX:
                  c_Retval = orc_Index.row() + 1;
                  break;
               case eNAME:
                  c_Retval = C_PuiSdHandler::h_GetInstance()->GetNamespace(rc_CurId);
                  break;
               case eVALUE_TYPE:
                  pc_UIElement = C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(rc_CurId.u32_NodeIndex,
                                                                                           rc_CurId.u32_DataPoolIndex,
                                                                                           rc_CurId.u32_ListIndex,
                                                                                           rc_CurId.u32_ElementIndex);
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     c_Retval = C_GtGetText::h_GetText("string");
                  }
                  else
                  {
                     pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_CurId.u32_NodeIndex,
                                                                                                rc_CurId.u32_DataPoolIndex,
                                                                                                rc_CurId.u32_ListIndex,
                                                                                                rc_CurId.u32_ElementIndex);
                     if (pc_OSCElement != NULL)
                     {
                        c_Retval = C_SdNdeDataPoolUtil::h_ConvertContentTypeToString(pc_OSCElement->c_Value.GetType());
                     }
                  }
                  break;
               case eARRAY_SIZE:
                  pc_OSCElement = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_CurId.u32_NodeIndex,
                                                                                             rc_CurId.u32_DataPoolIndex,
                                                                                             rc_CurId.u32_ListIndex,
                                                                                             rc_CurId.u32_ElementIndex);
                  if (pc_OSCElement->GetArray() == false)
                  {
                     c_Retval = "-";
                  }
                  else
                  {
                     c_Retval = static_cast<stw_types::sintn>(pc_OSCElement->GetArraySize());
                  }
                  break;
               case eDATA_POOL:
                  pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(rc_CurId.u32_NodeIndex,
                                                                                rc_CurId.u32_DataPoolIndex);
                  if (pc_DataPool != NULL)
                  {
                     c_Retval = pc_DataPool->c_Name.c_str();
                  }
                  break;
               case eTRANSMISSION_MODE:
                  c_Retval =
                     C_SyvDaPeUpdateModeTableModel::mh_TransmissionModeToString(rc_CurConfig.e_TransmissionMode);
                  break;
               case eCYCLIC_INTERVAL:
                  if (rc_CurConfig.e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
                  {
                     c_Retval =
                        C_SyvDaPeUpdateModeTableModel::m_RailIndexToString(rc_CurConfig.u8_RailIndex);
                  }
                  else
                  {
                     c_Retval = QString('-');
                  }
                  break;
               case eTHRESHOLD:
                  if (rc_CurConfig.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_CHANGE)
                  {
                     c_Retval =
                        C_SdNdeDataPoolContentUtil::h_ConvertContentToGeneric(rc_CurConfig.c_ChangeThreshold, 0);
                  }
                  else
                  {
                     c_Retval = QString('-');
                  }
                  break;
               case eUSAGE:
                  if (u32_Index < this->mc_Usage.size())
                  {
                     c_Retval = this->mc_Usage[u32_Index];
                  }
                  break;
               default:
                  //Unknown
                  break;
               }
            }
            else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
            {
               switch (e_Col)
               {
               case eNAME:
                  c_Retval = C_PuiSdHandler::h_GetInstance()->GetNamespace(rc_CurId);
                  break;
               default:
                  //Not necessary
                  break;
               }
            }
            else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
            {
               switch (e_Col)
               {
               case eNAME:
                  c_Retval = C_SdUtil::h_GetToolTipContentDpListElement(rc_CurId);
                  break;
               default:
                  //Not necessary
                  break;
               }
            }
            else if (osn_Role == static_cast<sintn>(Qt::EditRole))
            {
               switch (e_Col)
               {
               case eTRANSMISSION_MODE:
                  c_Retval = C_SyvDaPeUpdateModeTableModel::mh_TransmissionModeToIndex(rc_CurConfig.e_TransmissionMode);
                  break;
               case eCYCLIC_INTERVAL:
                  c_Retval = static_cast<sintn>(rc_CurConfig.u8_RailIndex);
                  break;
               case eTHRESHOLD:
                  c_Retval = C_SdNdeDataPoolContentUtil::h_ConvertContentToGeneric(rc_CurConfig.c_ChangeThreshold, 0);
                  break;
               default:
                  //Not necessary
                  break;
               }
            }
            else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
            {
               C_OSCNodeDataPool::E_Type e_DataPoolType;
               switch (e_Col)
               {
               case eICON:
                  if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(rc_CurId.u32_NodeIndex,
                                                                       rc_CurId.u32_DataPoolIndex,
                                                                       e_DataPoolType) == C_NO_ERR)
                  {
                     if (e_DataPoolType == C_OSCNodeDataPool::E_Type::eDIAG)
                     {
                        c_Retval = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconVariable.svg");
                     }
                     else
                     {
                        c_Retval = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconParameter.svg");
                     }
                  }
                  break;
               default:
                  //Not necessary
                  break;
               }
            }
            else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
            {
               const QColor c_Inactive1 = mc_STYLE_GUIDE_COLOR_8;
               const QColor c_Inactive2 = mc_STYLE_GUIDE_COLOR_10;
               const QColor c_Active = mc_STYLE_GUIDE_COLOR_6;
               //Stylesheets do not allow access of specific columns so we need to set it manually
               if (this->flags(orc_Index).testFlag(Qt::ItemIsEditable) == true)
               {
                  c_Retval = c_Active;
               }
               else
               {
                  switch (e_Col)
                  {
                  case eINDEX:
                  case eNAME:
                  case eVALUE_TYPE:
                  case eARRAY_SIZE:
                  case eDATA_POOL:
                  case eUSAGE:
                     c_Retval = c_Inactive1;
                     break;
                  default:
                     c_Retval = c_Inactive2;
                     break;
                  }
               }
            }
            else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
            {
               c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            }
            else if (osn_Role == msn_USER_ROLE_INTERACTION_MAXIMUM_VALUE)
            {
               //Max requested
               switch (e_Col)
               {
               case eTHRESHOLD:
                  switch (rc_CurConfig.c_ChangeThreshold.GetType())
                  {
                  case C_OSCNodeDataPoolContent::eUINT8:
                     c_Retval = static_cast<uint64>(std::numeric_limits<uint8>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eUINT16:
                     c_Retval = static_cast<uint64>(std::numeric_limits<uint16>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eUINT32:
                     c_Retval = static_cast<uint64>(std::numeric_limits<uint32>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eUINT64:
                     c_Retval = std::numeric_limits<uint64>::max();
                     break;
                  case C_OSCNodeDataPoolContent::eSINT8:
                     c_Retval = static_cast<sint64>(std::numeric_limits<sint8>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eSINT16:
                     c_Retval = static_cast<sint64>(std::numeric_limits<sint16>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eSINT32:
                     c_Retval = static_cast<sint64>(std::numeric_limits<sint32>::max());
                     break;
                  case C_OSCNodeDataPoolContent::eSINT64:
                     c_Retval = std::numeric_limits<sint64>::max();
                     break;
                  case C_OSCNodeDataPoolContent::eFLOAT32:
                     c_Retval = static_cast<float64>(std::numeric_limits<float32>::max());
                     break;
                  case C_OSCNodeDataPoolContent::E_Type::eFLOAT64:
                     c_Retval = std::numeric_limits<float64>::max();
                     break;
                  default:
                     //Nothing to do
                     break;
                  }
                  break;
               default:
                  //Not required
                  break;
               }
            }
            else
            {
               //Unknown
            }
         }
      }
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set data at index

   \param[in] orc_Index Index
   \param[in] orc_Value New data
   \param[in] osn_Role  Data role

   \return
   True  Success
   False Failure

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvDaPeUpdateModeTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                            const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (orc_Index.row() >= 0)
      {
         const uint32 u32_Index = orc_Index.row();
         if (u32_Index < this->mc_DataElementConfigurations.size())
         {
            C_PuiSvReadDataConfiguration & rc_CurConfig = this->mc_DataElementConfigurations[u32_Index];
            const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
            if (osn_Role == static_cast<sintn>(Qt::EditRole))
            {
               sint32 s32_Count = 0;
               QVector<sintn> c_Roles;
               switch (e_Col)
               {
               case eTRANSMISSION_MODE:
                  rc_CurConfig.e_TransmissionMode = C_SyvDaPeUpdateModeTableModel::mh_IndexToTransmissionMode(
                     orc_Value.toInt());
                  //Signal size change
                  for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_DataElementConfigurations.size(); ++u32_ItItem)
                  {
                     const C_PuiSvReadDataConfiguration & rc_Config = this->mc_DataElementConfigurations[u32_ItItem];
                     if (rc_Config.e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
                     {
                        ++s32_Count;
                     }
                  }
                  Q_EMIT this->SigTransmissionCountChange(s32_Count);
                  break;
               case eCYCLIC_INTERVAL:
                  rc_CurConfig.u8_RailIndex = static_cast<uint8>(orc_Value.toInt());
                  break;
               case eTHRESHOLD:
                  C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGeneric(orc_Value, rc_CurConfig.c_ChangeThreshold,
                                                                           0);
                  break;
               default:
                  //No action
                  break;
               }
               c_Roles << osn_Role;
               Q_EMIT dataChanged(orc_Index, orc_Index, c_Roles);
               q_Retval = true;
            }
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get item flags

   \param[in] orc_Index Index

   \return
   Item flags

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
Qt::ItemFlags C_SyvDaPeUpdateModeTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = Qt::NoItemFlags;
   if (orc_Index.isValid() == true)
   {
      if (orc_Index.row() >= 0)
      {
         const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
         if (u32_Index < this->mc_DataElementConfigurations.size())
         {
            const C_PuiSvReadDataConfiguration & rc_CurConfig = this->mc_DataElementConfigurations[u32_Index];
            const C_SyvDaPeUpdateModeTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
            const C_OSCNodeDataPoolListElementId * pc_Id;
            //Default
            c_Retval =  Qt::ItemIsSelectable;
            switch (e_Col)
            {
            case eINDEX:
            case eICON:
            case eNAME:
            case eVALUE_TYPE:
            case eARRAY_SIZE:
            case eDATA_POOL:
            case eUSAGE:
               //For design
               c_Retval = c_Retval | Qt::ItemIsEnabled;
               break;
            case eTRANSMISSION_MODE:
               pc_Id = this->GetIndex(orc_Index.row());
               if (pc_Id != NULL)
               {
                  const C_OSCNodeDataPoolListElement * const pc_Element =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(pc_Id->u32_NodeIndex,
                                                                                pc_Id->u32_DataPoolIndex,
                                                                                pc_Id->u32_ListIndex,
                                                                                pc_Id->u32_ElementIndex);
                  if ((((pc_Element->GetArray() == false) &&
                        (pc_Element->GetType() != C_OSCNodeDataPoolContent::eFLOAT64)) &&
                       (pc_Element->GetType() != C_OSCNodeDataPoolContent::eUINT64)) &&
                      (pc_Element->GetType() != C_OSCNodeDataPoolContent::eSINT64))
                  {
                     c_Retval = c_Retval | Qt::ItemIsEnabled | Qt::ItemIsEditable;
                  }
               }
               break;
            case eCYCLIC_INTERVAL:
               //Only editable in special case
               if (rc_CurConfig.e_TransmissionMode != C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER)
               {
                  c_Retval = c_Retval | Qt::ItemIsEnabled | Qt::ItemIsEditable;
               }
               break;
            case eTHRESHOLD:
               //Only editable in special case
               if (rc_CurConfig.e_TransmissionMode == C_PuiSvReadDataConfiguration::eTM_ON_CHANGE)
               {
                  c_Retval = c_Retval | Qt::ItemIsEnabled | Qt::ItemIsEditable;
               }
               break;
            default:
               break;
            }
         }
      }
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Column to enum conversion

   \param[in]  os32_Column Column

   \return
   Enum value

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableModel::E_Columns C_SyvDaPeUpdateModeTableModel::h_ColumnToEnum(const sint32 os32_Column)
{
   C_SyvDaPeUpdateModeTableModel::E_Columns e_Retval = eICON;
   switch (os32_Column)
   {
   case 0:
      e_Retval = eINDEX;
      break;
   case 1:
      e_Retval = eICON;
      break;
   case 2:
      e_Retval = eNAME;
      break;
   case 3:
      e_Retval = eVALUE_TYPE;
      break;
   case 4:
      e_Retval = eARRAY_SIZE;
      break;
   case 5:
      e_Retval = eDATA_POOL;
      break;
   case 6:
      e_Retval = eTRANSMISSION_MODE;
      break;
   case 7:
      e_Retval = eCYCLIC_INTERVAL;
      break;
   case 8:
      e_Retval = eTHRESHOLD;
      break;
   case 9:
      e_Retval = eUSAGE;
      break;
   default:
      break;
   }

   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Enum to column conversion

   \param[in] oe_Value Enum value

   \return
   Column
   -1 Error

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDaPeUpdateModeTableModel::h_EnumToColumn(const E_Columns oe_Value)
{
   sint32 s32_Retval;

   switch (oe_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eICON:
      s32_Retval = 1;
      break;
   case eNAME:
      s32_Retval = 2;
      break;
   case eVALUE_TYPE:
      s32_Retval = 3;
      break;
   case eARRAY_SIZE:
      s32_Retval = 4;
      break;
   case eDATA_POOL:
      s32_Retval = 5;
      break;
   case eTRANSMISSION_MODE:
      s32_Retval = 6;
      break;
   case eCYCLIC_INTERVAL:
      s32_Retval = 7;
      break;
   case eTHRESHOLD:
      s32_Retval = 8;
      break;
   case eUSAGE:
      s32_Retval = 9;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index

   \param[in] os32_Index Index

   \return
   NULL Element not found
   Else Valid element


   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_OSCNodeDataPoolListElementId * C_SyvDaPeUpdateModeTableModel::GetIndex(const sint32 os32_Index) const
{
   const C_OSCNodeDataPoolListElementId * pc_Retval = NULL;

   if (os32_Index >= 0)
   {
      const uint32 u32_Index = static_cast<uint32>(os32_Index);
      if (u32_Index < this->mc_UniqueDataElementIds.size())
      {
         pc_Retval = &this->mc_UniqueDataElementIds[u32_Index];
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init

   \param[in] ou32_ViewIndex View index
   \param[in] ou32_NodeIndex Node index

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeUpdateModeTableModel::m_Init(const uint32 ou32_NodeIndex)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   this->beginResetModel();
   this->mc_UniqueDataElementIds.clear();
   this->mc_DataElementConfigurations.clear();
   if (pc_View != NULL)
   {
      const QMap<C_OSCNodeDataPoolListElementId,
                 C_PuiSvReadDataConfiguration> & rc_AllReadRailAssignments = pc_View->GetReadRailAssignments();
      const std::vector<C_PuiSvDashboard> & rc_Dashboards = pc_View->GetDashboards();
      //Sort by node
      for (QMap<C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration>::const_iterator c_ItAssignment =
              rc_AllReadRailAssignments.begin();
           c_ItAssignment != rc_AllReadRailAssignments.end(); ++c_ItAssignment)
      {
         if (c_ItAssignment.key().u32_NodeIndex == ou32_NodeIndex)
         {
            //Filter param entries
            if (pc_View->CheckNonParamReadUsage(c_ItAssignment.key()) == true)
            {
               this->mc_UniqueDataElementIds.push_back(c_ItAssignment.key());
               this->mc_DataElementConfigurations.push_back(c_ItAssignment.value());
            }
         }
      }
      //Fill usage
      this->mc_Usage.clear();
      this->mc_Usage.resize(this->mc_UniqueDataElementIds.size(), "");
      for (uint32 u32_ItDashboard = 0; u32_ItDashboard < rc_Dashboards.size(); ++u32_ItDashboard)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(u32_ItDashboard);
         if (pc_Dashboard != NULL)
         {
            std::vector<uint32> c_Count;
            std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;

            c_Count.resize(this->mc_UniqueDataElementIds.size(), 0);
            pc_Dashboard->GetAllWidgetItems(c_Widgets);
            //Search for usage in dashboard
            //All widgets
            for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
            {
               const C_PuiSvDbWidgetBase * const pc_CurWidget = c_Widgets[u32_ItWidget];
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               if (((pc_CurWidget != NULL) && (pc_CurWidget->IsReadElement() == true)) &&
                   (dynamic_cast<const C_PuiSvDbParam * const>(pc_CurWidget) == NULL))
               {
                  //All data elements in widget
                  for (uint32 u32_ItConfig = 0; u32_ItConfig < pc_CurWidget->c_DataPoolElementsConfig.size();
                       ++u32_ItConfig)
                  {
                     const C_PuiSvDbNodeDataElementConfig & rc_Config =
                        pc_CurWidget->c_DataPoolElementsConfig[u32_ItConfig];
                     if (rc_Config.c_ElementId.GetIsValid() == true)
                     {
                        //All data elements in model
                        for (uint32 u32_ItDataElement = 0; u32_ItDataElement < this->mc_UniqueDataElementIds.size();
                             ++u32_ItDataElement)
                        {
                           const C_OSCNodeDataPoolListElementId & rc_DataElement =
                              this->mc_UniqueDataElementIds[u32_ItDataElement];
                           //Match
                           if (rc_Config.c_ElementId == rc_DataElement)
                           {
                              uint32 & ru32_Count = c_Count[u32_ItDataElement];
                              ++ru32_Count;
                           }
                        }
                     }
                  }
               }
            }
            //Apply any usages (if necessary)
            for (uint32 u32_ItDataElement = 0; u32_ItDataElement < this->mc_UniqueDataElementIds.size();
                 ++u32_ItDataElement)
            {
               if (c_Count[u32_ItDataElement] > 0)
               {
                  this->mc_Usage[u32_ItDataElement] +=
                     QString("%1(%2);").arg(pc_Dashboard->GetName()).arg(c_Count[u32_ItDataElement]);
               }
            }
         }
      }
   }
   this->endResetModel();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get transmission mode as string

   \param[in] oe_TransmissionMode Transmission mode

   \return
   Readable string

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvDaPeUpdateModeTableModel::mh_TransmissionModeToString(
   const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode)
{
   QString c_Retval;

   switch (oe_TransmissionMode)
   {
   case C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER:
      c_Retval = C_GtGetText::h_GetText("On Trigger");
      break;
   case C_PuiSvReadDataConfiguration::eTM_CYCLIC:
      c_Retval = C_GtGetText::h_GetText("Cyclic");
      break;
   case C_PuiSvReadDataConfiguration::eTM_ON_CHANGE:
      c_Retval = C_GtGetText::h_GetText("On Change");
      break;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get rail index as string

   \param[in] u8_RailIndex Rail index

   \return
   Readable string

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvDaPeUpdateModeTableModel::m_RailIndexToString(const uint8 ou8_RailIndex) const
{
   QString c_Retval;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      switch (ou8_RailIndex)
      {
      case 0U:
         c_Retval = QString(C_GtGetText::h_GetText("Fast (%1 ms)")).arg(pc_View->GetUpdateRateFast());
         break;
      case 1U:
         c_Retval = QString(C_GtGetText::h_GetText("Medium (%1 ms)")).arg(pc_View->GetUpdateRateMedium());
         break;
      case 2U:
         c_Retval = QString(C_GtGetText::h_GetText("Slow (%1 ms)")).arg(pc_View->GetUpdateRateSlow());
         break;
      default:
         c_Retval = "";
         break;
      }
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get index for specified transmission mode

   \param[in] oe_TransmissionMode Transmission mode

   \return
   Transmission mode index (Combo box)

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaPeUpdateModeTableModel::mh_TransmissionModeToIndex(
   const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode)
{
   sintn sn_Retval;

   switch (oe_TransmissionMode)
   {
   case C_PuiSvReadDataConfiguration::eTM_CYCLIC:
      sn_Retval = 0;
      break;
   case C_PuiSvReadDataConfiguration::eTM_ON_CHANGE:
      sn_Retval = 1;
      break;
   case C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER:
      sn_Retval = 2;
      break;
   default:
      sn_Retval = 0;
   }
   return sn_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get transmission mode for specified index

   \param[in] osn_Index Transmission mode index (Combo box)

   \return
   Transmission mode

   \created     11.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvReadDataConfiguration::E_TransmissionMode C_SyvDaPeUpdateModeTableModel::mh_IndexToTransmissionMode(
   const sintn osn_Index)
{
   C_PuiSvReadDataConfiguration::E_TransmissionMode e_Retval;
   switch (osn_Index)
   {
   case 0:
      e_Retval = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
      break;
   case 1:
      e_Retval = C_PuiSvReadDataConfiguration::eTM_ON_CHANGE;
      break;
   case 2:
      e_Retval = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
      break;
   default:
      e_Retval = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
      break;
   }
   return e_Retval;
}
