//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model part for add signals tree

   Model part for add signals tree

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "C_SdUtil.hpp"
#include "TglUtils.hpp"
#include "constants.hpp"
#include "stwerrors.hpp"
#include "C_CieUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_TblTreItem.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscImportEdsDcf.hpp"
#include "C_SdBueCoAddSignalsModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueCoAddSignalsModel::C_SdBueCoAddSignalsModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   mc_IconSignal(":/images/system_definition/IconSignal.svg")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueCoAddSignalsModel::~C_SdBueCoAddSignalsModel()
{
   this->PrepareCleanUp();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set index

   \param[in]  orc_MessageId  Message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::SetIndex(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   this->mc_MessageId = orc_MessageId;
   this->m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cleans up the last model configuration as preparation for the entire deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::PrepareCleanUp()
{
   this->beginResetModel();
   if (this->mpc_InvisibleRootItem != NULL)
   {
      delete (this->mpc_InvisibleRootItem);
      this->mpc_InvisibleRootItem = NULL;
   }
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data for index

   \param[in]  ou32_ObjectIndex  Object index
   \param[in]  ou32_SignalIndex  Signal index

   \return
   Data for index
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerMappableSignal * C_SdBueCoAddSignalsModel::GetDataForIndex(const uint32_t ou32_ObjectIndex,
                                                                                    const uint32_t ou32_SignalIndex)
const
{
   const C_OscCanOpenManagerMappableSignal * pc_Retval = NULL;
   const std::map<uint32_t,
                  std::map<uint32_t, uint32_t> >::const_iterator c_ItObject =
      this->mc_MapObjectIndexToVectorIndex.find(ou32_ObjectIndex);

   if (c_ItObject != this->mc_MapObjectIndexToVectorIndex.cend())
   {
      const std::map<uint32_t, uint32_t>::const_iterator c_ItItem =
         c_ItObject->second.find(ou32_SignalIndex);

      if (c_ItItem != c_ItObject->second.cend())
      {
         const std::vector<C_OscCanOpenManagerMappableSignal> * const pc_MappableSignals = this->m_GetMappableSignals();
         if ((pc_MappableSignals != NULL) && (c_ItItem->second < pc_MappableSignals->size()))
         {
            pc_Retval = &(*pc_MappableSignals)[c_ItItem->second];
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get unique indices

   \param[in]  orc_ModelIndices  Model indices

   \return
   Unique indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<uint32_t, std::vector<uint32_t> > C_SdBueCoAddSignalsModel::h_GetUniqueIndices(
   const QModelIndexList & orc_ModelIndices)
{
   std::map<uint32_t, std::vector<uint32_t> > c_Retval;
   for (QModelIndexList::ConstIterator c_It = orc_ModelIndices.cbegin(); c_It != orc_ModelIndices.cend(); ++c_It)
   {
      uint32_t u32_ObjectIndex;
      uint32_t u32_SignalIndex;
      bool q_IsSignal;
      C_SdBueCoAddSignalsModel::mh_DecodeIndex(*c_It, u32_ObjectIndex, u32_SignalIndex, q_IsSignal);
      tgl_assert(q_IsSignal == true);
      c_Retval[u32_ObjectIndex].push_back(u32_SignalIndex);
   }
   for (std::map<uint32_t, std::vector<uint32_t> >::iterator c_It = c_Retval.begin();
        c_It != c_Retval.end(); ++c_It)
   {
      C_Uti::h_Uniqueify(c_It->second);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueCoAddSignalsModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 3;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation     Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueCoAddSignalsModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                              const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueCoAddSignalsModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eLENGTH:
            c_Retval = C_GtGetText::h_GetText("Length[Bit]");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Index");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eLENGTH:
            c_Retval = C_GtGetText::h_GetText("Length[Bit]");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("CANopen object dictionary entry index");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("CANopen object dictionary entry name");
            break;
         case eLENGTH:
            c_Retval = C_GtGetText::h_GetText("CANopen object dictionary entry bit length");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueCoAddSignalsModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, os32_Role);

   // first columns are handled by C_TblTreItem
   if (orc_Index.column() > 0)
   {
      const C_SdBueCoAddSignalsModel::E_Columns e_Col =
         C_SdBueCoAddSignalsModel::h_ColumnToEnum(orc_Index.column());
      uint32_t u32_ObjectIndex;
      uint32_t u32_SignalIndex;
      bool q_IsSignal;
      C_SdBueCoAddSignalsModel::mh_DecodeIndex(orc_Index, u32_ObjectIndex, u32_SignalIndex, q_IsSignal);
      this->m_GetData(e_Col, u32_ObjectIndex, u32_SignalIndex, q_IsSignal, os32_Role, c_Retval);
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueCoAddSignalsModel::E_Columns C_SdBueCoAddSignalsModel::h_ColumnToEnum(const int32_t & ors32_Column)
{
   C_SdBueCoAddSignalsModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eINDEX;
      break;
   case 1:
      e_Retval = eNAME;
      break;
   case 2:
      e_Retval = eLENGTH;
      break;
   default:
      //Unknown
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
int32_t C_SdBueCoAddSignalsModel::h_EnumToColumn(const C_SdBueCoAddSignalsModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eINDEX:
      s32_Retval = 0;
      break;
   case eNAME:
      s32_Retval = 1;
      break;
   case eLENGTH:
      s32_Retval = 2;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_Init(void)
{
   const C_OscCanOpenObjectDictionary * const pc_EdsDictionary = this->m_GetEdsDictionary();

   if (pc_EdsDictionary != NULL)
   {
      C_TblTreItem * const pc_RootItem = new C_TblTreItem();

      tgl_assert(this->mpc_InvisibleRootItem == NULL);
      this->beginResetModel();
      this->m_InitObjectMap();
      this->mpc_InvisibleRootItem = pc_RootItem;
      this->m_InitTopLevel(*pc_RootItem);
      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init object map
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitObjectMap()
{
   const std::vector<C_OscCanOpenManagerMappableSignal> * const pc_MappableSignals = this->m_GetMappableSignals();
   const C_OscCanOpenObjectDictionary * const pc_EdsDictionary = this->m_GetEdsDictionary();

   this->mc_MapObjectIndexToVectorIndex.clear();
   if ((pc_MappableSignals != NULL) && (pc_EdsDictionary != NULL))
   {
      for (uint32_t u32_It = 0UL; u32_It < pc_MappableSignals->size(); ++u32_It)
      {
         const C_OscCanOpenManagerMappableSignal & rc_Signal = (*pc_MappableSignals)[u32_It];
         if (this->m_CheckSignalRelevant(rc_Signal, *pc_EdsDictionary))
         {
            const std::map<uint32_t,
                           std::map<uint32_t, uint32_t> >::iterator c_ItObject =
               this->mc_MapObjectIndexToVectorIndex.find(rc_Signal.c_SignalData.u16_CanOpenManagerObjectDictionaryIndex);

            if (c_ItObject != this->mc_MapObjectIndexToVectorIndex.cend())
            {
               const std::map<uint32_t, uint32_t>::iterator c_ItItem =
                  c_ItObject->second.find(rc_Signal.c_SignalData.u8_CanOpenManagerObjectDictionarySubIndex);

               if (c_ItItem != c_ItObject->second.cend())
               {
                  //Should not happen
                  tgl_assert(false);
               }
               else
               {
                  //Add to existing
                  c_ItObject->second[rc_Signal.c_SignalData.u8_CanOpenManagerObjectDictionarySubIndex] = u32_It;
               }
            }
            else
            {
               std::map<uint32_t, uint32_t> c_FreshMap;
               c_FreshMap[rc_Signal.c_SignalData.u8_CanOpenManagerObjectDictionarySubIndex] = u32_It;
               //New
               this->mc_MapObjectIndexToVectorIndex[rc_Signal.c_SignalData.u16_CanOpenManagerObjectDictionaryIndex] =
                  c_FreshMap;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init top level items

   \param[in,out]  orc_RootNode  Root node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitTopLevel(C_TblTreItem & orc_RootNode)
{
   for (std::map<uint32_t,
                 std::map<uint32_t,
                          uint32_t> >::const_iterator c_It = this->mc_MapObjectIndexToVectorIndex.begin();
        c_It != this->mc_MapObjectIndexToVectorIndex.end(); ++c_It)
   {
      C_TblTreItem * const pc_ObjectItem = new C_TblTreItem();
      m_InitObjectNode(*pc_ObjectItem, c_It->first);
      orc_RootNode.AddChild(pc_ObjectItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init object node

   \param[in,out]  orc_ObjectNode      Object node
   \param[in]      ou32_ObjectIndex    Object index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitObjectNode(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex)
{
   m_InitObjectNodeContent(orc_ObjectNode, ou32_ObjectIndex);
   m_InitObjectNodeChildren(orc_ObjectNode, ou32_ObjectIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init object node content

   \param[in,out]  orc_ObjectNode      Object node
   \param[in]      ou32_ObjectIndex    Object index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitObjectNodeContent(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex)
{
   orc_ObjectNode.c_Name = QString::number(ou32_ObjectIndex, 16).toUpper();
   orc_ObjectNode.q_Enabled = true;
   orc_ObjectNode.q_Selectable = false;

   orc_ObjectNode.u32_Index = ou32_ObjectIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init object node children

   \param[in,out]  orc_ObjectNode      Object node
   \param[in]      ou32_ObjectIndex    Object index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitObjectNodeChildren(C_TblTreItem & orc_ObjectNode, const uint32_t ou32_ObjectIndex)
{
   const std::map<uint32_t,
                  std::map<uint32_t,
                           uint32_t> >::const_iterator c_ItMappableObjects =
      this->mc_MapObjectIndexToVectorIndex.find(ou32_ObjectIndex);

   tgl_assert(c_ItMappableObjects != this->mc_MapObjectIndexToVectorIndex.end());
   if (c_ItMappableObjects != this->mc_MapObjectIndexToVectorIndex.end())
   {
      for (std::map<uint32_t,
                    uint32_t>::const_iterator c_ItEntry = c_ItMappableObjects->second.cbegin();
           c_ItEntry != c_ItMappableObjects->second.cend(); ++c_ItEntry)
      {
         C_TblTreItem * const pc_SignalItem = new C_TblTreItem();
         m_InitSignalNodeContent(*pc_SignalItem, ou32_ObjectIndex, c_ItEntry->first);
         orc_ObjectNode.AddChild(pc_SignalItem);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init signal node content

   \param[in,out]  orc_SignalNode         Signal node
   \param[in]      ou32_ObjectIndex       Object index
   \param[in]      ou32_ObjectSubIndex    Object sub index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitSignalNodeContent(C_TblTreItem & orc_SignalNode, const uint32_t ou32_ObjectIndex,
                                                       const uint32_t ou32_ObjectSubIndex)
{
   orc_SignalNode.c_Name = C_SdUtil::h_GetCanOpenSignalObjectIndex(ou32_ObjectIndex, ou32_ObjectSubIndex);
   orc_SignalNode.c_Icon = this->mc_IconSignal;

   orc_SignalNode.q_Enabled = true;
   orc_SignalNode.q_Selectable = true;

   orc_SignalNode.u32_Index = ou32_ObjectSubIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decode index

   \param[in]   orc_ModelIndex      Model index
   \param[out]  oru32_ObjectIndex   Object index
   \param[out]  oru32_SignalIndex   Signal index
   \param[in]   orq_IsSignal        Flag: is signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::mh_DecodeIndex(const QModelIndex & orc_ModelIndex, uint32_t & oru32_ObjectIndex,
                                              uint32_t & oru32_SignalIndex, bool & orq_IsSignal)
{
   if (orc_ModelIndex.isValid())
   {
      //lint -e{9079} Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem *>(orc_ModelIndex.internalPointer());
      tgl_assert(pc_TreeItem != NULL);
      if (pc_TreeItem != NULL)
      {
         const QModelIndex c_ParentModelIndex = orc_ModelIndex.parent();
         if (c_ParentModelIndex.isValid())
         {
            //lint -e{9079} Result of Qt interface restrictions, set by index function
            const C_TblTreItem * const pc_ParentTreeItem =
               static_cast<const C_TblTreItem *>(c_ParentModelIndex.internalPointer());
            tgl_assert(pc_ParentTreeItem != NULL);
            if (pc_ParentTreeItem != NULL)
            {
               oru32_ObjectIndex = pc_ParentTreeItem->u32_Index;
            }
            oru32_SignalIndex = pc_TreeItem->u32_Index;
            orq_IsSignal = true;
         }
         else
         {
            oru32_ObjectIndex = pc_TreeItem->u32_Index;
            oru32_SignalIndex = 0UL;
            orq_IsSignal = false;
         }
      }
   }
   else
   {
      oru32_ObjectIndex = 0UL;
      oru32_SignalIndex = 0UL;
      orq_IsSignal = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]      oe_Column           Column
   \param[in]      ou32_ObjectIndex    Object index
   \param[in]      ou32_SignalIndex    Signal index
   \param[in]      oq_IsSignal         Flag: is signal
   \param[in]      os32_Role           Data role
   \param[in,out]  orc_Output          Output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_GetData(const E_Columns oe_Column, const uint32_t ou32_ObjectIndex,
                                         const uint32_t ou32_SignalIndex, const bool oq_IsSignal,
                                         const int32_t os32_Role, QVariant & orc_Output) const
{
   if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
   {
      const C_OscCanOpenObjectDictionary * pc_EdsDictionary;
      if (oq_IsSignal)
      {
         const std::map<uint32_t,
                        std::map<uint32_t, uint32_t> >::const_iterator c_ItObject =
            this->mc_MapObjectIndexToVectorIndex.find(ou32_ObjectIndex);

         tgl_assert(c_ItObject != this->mc_MapObjectIndexToVectorIndex.cend());
         if (c_ItObject != this->mc_MapObjectIndexToVectorIndex.cend())
         {
            const std::map<uint32_t, uint32_t>::const_iterator c_ItItem =
               c_ItObject->second.find(ou32_SignalIndex);

            tgl_assert(c_ItItem != c_ItObject->second.cend());
            if (c_ItItem != c_ItObject->second.cend())
            {
               const std::vector<C_OscCanOpenManagerMappableSignal> * const pc_MappableSignals =
                  this->m_GetMappableSignals();

               tgl_assert(pc_MappableSignals != NULL);
               if (pc_MappableSignals != NULL)
               {
                  tgl_assert(c_ItItem->second < pc_MappableSignals->size());
                  if (c_ItItem->second < pc_MappableSignals->size())
                  {
                     const C_OscCanOpenManagerMappableSignal & rc_ResultEntry = (*pc_MappableSignals)[c_ItItem->second];
                     // show data for column
                     switch (oe_Column)
                     {
                     case eINDEX:
                        // name is handled by tree item name
                        break;
                     case eNAME:
                        pc_EdsDictionary = this->m_GetEdsDictionary();
                        if (pc_EdsDictionary != NULL)
                        {
                           const C_OscCanOpenObject * const pc_Object = C_SdBueCoAddSignalsModel::mh_GetCanOpenObject(
                              *pc_EdsDictionary,
                              ou32_ObjectIndex,
                              ou32_SignalIndex);
                           if (pc_Object != NULL)
                           {
                              orc_Output = C_OscImportEdsDcf::h_GetObjectName(*pc_Object).c_str();
                           }
                        }
                        break;
                     case eLENGTH:
                        orc_Output = rc_ResultEntry.c_SignalData.u16_ComBitLength;
                        break;
                     default:
                        tgl_assert(false);
                        break;
                     }
                  }
               }
            }
         }
      }
      else
      {
         // show data for column
         switch (oe_Column)
         {
         case eINDEX:
            // name is handled by tree item name
            break;
         case eNAME:
            pc_EdsDictionary = this->m_GetEdsDictionary();
            if (pc_EdsDictionary != NULL)
            {
               const C_OscCanOpenObject * const pc_Object =
                  pc_EdsDictionary->GetCanOpenObject(static_cast<uint16_t>(ou32_ObjectIndex));
               if (pc_Object != NULL)
               {
                  orc_Output = C_OscImportEdsDcf::h_GetObjectName(*pc_Object).c_str();
               }
            }
            break;
         case eLENGTH:
            orc_Output = "";
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
   }
   else
   {
      // Use default
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get EDS dictionary

   \return
   NULL Not found
   Else EDS dictionary
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenObjectDictionary * C_SdBueCoAddSignalsModel::m_GetEdsDictionary() const
{
   const C_OscCanOpenObjectDictionary * pc_Retval = NULL;
   const C_OscCanOpenManagerDeviceInfo * const pc_Device = this->m_GetDeviceInfo();

   if (pc_Device != NULL)
   {
      pc_Retval = &pc_Device->c_EdsFileContent;
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get mappable signals

   \return
   NULL Not found
   Else Mappable signals
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscCanOpenManagerMappableSignal> * C_SdBueCoAddSignalsModel::m_GetMappableSignals() const
{
   const std::vector<C_OscCanOpenManagerMappableSignal> * pc_Retval = NULL;

   const C_OscCanOpenManagerDeviceInfo * const pc_Device = this->m_GetDeviceInfo();

   if (pc_Device != NULL)
   {
      pc_Retval = &pc_Device->c_EdsFileMappableSignals;
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get device info

   \return
   NULL Not found
   Else Device info
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerDeviceInfo * C_SdBueCoAddSignalsModel::m_GetDeviceInfo() const
{
   uint8_t u8_InterfaceNumber;
   const C_OscCanOpenManagerDeviceInfo * pc_Retval = NULL;

   if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(this->mc_MessageId.u32_NodeIndex,
                                                                       this->mc_MessageId.u32_InterfaceIndex,
                                                                       u8_InterfaceNumber) == C_NO_ERR)
   {
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
      if (pc_Message != NULL)
      {
         pc_Retval =
            C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mc_MessageId.u32_NodeIndex,
                                                                     u8_InterfaceNumber,
                                                                     pc_Message->c_CanOpenManagerOwnerNodeIndex);
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen object

   \param[in]  orc_Dictionary    Dictionary
   \param[in]  ou32_ObjectIndex  Object index
   \param[in]  ou32_SignalIndex  Signal index

   \return
   CANopen object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenObject * C_SdBueCoAddSignalsModel::mh_GetCanOpenObject(
   const C_OscCanOpenObjectDictionary & orc_Dictionary, const uint32_t ou32_ObjectIndex,
   const uint32_t ou32_SignalIndex)
{
   const C_OscCanOpenObject * pc_Object;

   if (ou32_SignalIndex == 0U)
   {
      //Object
      pc_Object = orc_Dictionary.GetCanOpenObject(static_cast<uint16_t>(ou32_ObjectIndex));
   }
   else
   {
      //Object with sub index
      pc_Object = orc_Dictionary.GetCanOpenSubIndexObject(
         static_cast<uint16_t>(ou32_ObjectIndex), static_cast<uint8_t>(ou32_SignalIndex));
   }
   return pc_Object;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check signal relevant

   \param[in]  orc_Signal        Signal
   \param[in]  orc_Dictionary    Dictionary

   \return
   Flags

   \retval   True    Signal relevant
   \retval   False   Signal not relevant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueCoAddSignalsModel::m_CheckSignalRelevant(const C_OscCanOpenManagerMappableSignal & orc_Signal,
                                                     const C_OscCanOpenObjectDictionary & orc_Dictionary) const
{
   bool q_Retval = false;
   const C_OscCanOpenObject * const pc_Object = C_SdBueCoAddSignalsModel::mh_GetCanOpenObject(
      orc_Dictionary,
      static_cast<uint32_t>(orc_Signal.c_SignalData.u16_CanOpenManagerObjectDictionaryIndex),
      static_cast<uint32_t>(orc_Signal.c_SignalData.u8_CanOpenManagerObjectDictionarySubIndex));

   if (pc_Object != NULL)
   {
      if (((this->mc_MessageId.q_MessageIsTx) && (pc_Object->IsMappableIntoRxPdo())) ||
          ((!this->mc_MessageId.q_MessageIsTx) && (pc_Object->IsMappableIntoTxPdo())))
      {
         q_Retval = true;
      }
   }
   return q_Retval;
}
