//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model part for add signals tree

   Model part for add signals tree

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "TGLUtils.h"
#include "constants.h"
#include "stwerrors.h"
#include "C_CieUtil.h"
#include "C_GtGetText.h"
#include "C_TblTreItem.h"
#include "C_PuiSdHandler.h"
#include "C_OSCImportEdsDcf.h"
#include "C_SdBueCoAddSignalsModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
void C_SdBueCoAddSignalsModel::SetIndex(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
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
   if (this->mpc_InvisibleRootItem != NULL)
   {
      delete (this->mpc_InvisibleRootItem);
      this->mpc_InvisibleRootItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data for index

   \param[in]  ou32_ObjectIndex  Object index
   \param[in]  ou32_SignalIndex  Signal index

   \return
   Data for index
*/
//----------------------------------------------------------------------------------------------------------------------
const C_SdBueCoAddSignalsResultEntry * C_SdBueCoAddSignalsModel::GetDataForIndex(const uint32 ou32_ObjectIndex,
                                                                                 const uint32 ou32_SignalIndex) const
{
   const C_SdBueCoAddSignalsResultEntry * pc_Retval = NULL;
   const std::map<stw_types::uint32,
                  std::vector<C_SdBueCoAddSignalsResultEntry> >::const_iterator c_ItMappableObjectContent =
      this->mc_MappableObjectContent.find(ou32_ObjectIndex);

   if (c_ItMappableObjectContent != this->mc_MappableObjectContent.cend())
   {
      if (ou32_SignalIndex < c_ItMappableObjectContent->second.size())
      {
         pc_Retval = &c_ItMappableObjectContent->second[ou32_SignalIndex];
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
std::map<uint32, std::vector<uint32> > C_SdBueCoAddSignalsModel::h_GetUniqueIndices(
   const QModelIndexList & orc_ModelIndices)
{
   std::map<uint32, std::vector<uint32> > c_Retval;
   for (QModelIndexList::ConstIterator c_It = orc_ModelIndices.cbegin(); c_It != orc_ModelIndices.cend(); ++c_It)
   {
      uint32 u32_ObjectIndex;
      uint32 u32_SignalIndex;
      bool q_IsSignal;
      C_SdBueCoAddSignalsModel::mh_DecodeIndex(*c_It, u32_ObjectIndex, u32_SignalIndex, q_IsSignal);
      tgl_assert(q_IsSignal == true);
      c_Retval[u32_ObjectIndex].push_back(u32_SignalIndex);
   }
   for (std::map<stw_types::uint32, std::vector<stw_types::uint32> >::iterator c_It = c_Retval.begin();
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
stw_types::sintn C_SdBueCoAddSignalsModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 3;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueCoAddSignalsModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                              const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractItemModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueCoAddSignalsModel::E_Columns e_Col = h_ColumnToEnum(osn_Section);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
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
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
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
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
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
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
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
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueCoAddSignalsModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval = C_TblTreModel::data(orc_Index, osn_Role);

   // first columns are handled by C_TblTreItem
   if (orc_Index.column() > 0)
   {
      const C_SdBueCoAddSignalsModel::E_Columns e_Col =
         C_SdBueCoAddSignalsModel::h_ColumnToEnum(orc_Index.column());
      uint32 u32_ObjectIndex;
      uint32 u32_SignalIndex;
      bool q_IsSignal;
      C_SdBueCoAddSignalsModel::mh_DecodeIndex(orc_Index, u32_ObjectIndex, u32_SignalIndex, q_IsSignal);
      this->m_GetData(e_Col, u32_ObjectIndex, u32_SignalIndex, q_IsSignal, osn_Role, c_Retval);
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
C_SdBueCoAddSignalsModel::E_Columns C_SdBueCoAddSignalsModel::h_ColumnToEnum(const sint32 & ors32_Column)
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
sint32 C_SdBueCoAddSignalsModel::h_EnumToColumn(const C_SdBueCoAddSignalsModel::E_Columns & ore_Value)
{
   sint32 s32_Retval;

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
   const C_OSCCanOpenObjectDictionary * const pc_EdsDictionary = this->m_GetEdsDictionary();

   if (pc_EdsDictionary != NULL)
   {
      C_TblTreItem * const pc_RootItem = new C_TblTreItem();

      tgl_assert(this->mpc_InvisibleRootItem == NULL);
      this->beginResetModel();
      pc_EdsDictionary->GetMappableObjects(this->mc_MappableObjects);
      this->m_InitResult(*pc_EdsDictionary);
      this->mpc_InvisibleRootItem = pc_RootItem;
      this->m_InitTopLevel(*pc_RootItem);
      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init result

   \param[in]  orc_EdsDictionary    EDS dictionary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitResult(const C_OSCCanOpenObjectDictionary & orc_EdsDictionary)
{
   for (std::map<uint32, std::vector<uint32> >::const_iterator c_It = this->mc_MappableObjects.begin();
        c_It != this->mc_MappableObjects.end(); ++c_It)
   {
      std::vector<C_SdBueCoAddSignalsResultEntry> c_Entries;
      c_Entries.reserve(c_It->second.size());
      for (uint32 u32_ItSig = 0UL; u32_ItSig < c_It->second.size(); ++u32_ItSig)
      {
         C_SdBueCoAddSignalsResultEntry c_Entry;
         tgl_assert(C_OSCImportEdsDcf::h_ParseSignalContent(orc_EdsDictionary.c_Objects, c_It->first,
                                                            c_It->second[u32_ItSig], 0UL,
                                                            true, c_Entry.c_SignalData,
                                                            c_Entry.c_DatapoolData,
                                                            c_Entry.q_AutoMinMaxUsed) == C_NO_ERR);
         C_CieUtil::h_AdaptName(c_Entry.c_DatapoolData.c_Name, c_Entry.c_DatapoolData.c_Comment);
         c_Entries.push_back(c_Entry);
      }
      this->mc_MappableObjectContent[c_It->first] = c_Entries;
   }
   tgl_assert(this->mc_MappableObjects.size() == this->mc_MappableObjectContent.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init top level items

   \param[in,out]  orc_RootNode  Root node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitTopLevel(C_TblTreItem & orc_RootNode)
{
   for (std::map<uint32, std::vector<uint32> >::const_iterator c_It = this->mc_MappableObjects.begin();
        c_It != this->mc_MappableObjects.end(); ++c_It)
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
void C_SdBueCoAddSignalsModel::m_InitObjectNode(C_TblTreItem & orc_ObjectNode, const uint32 ou32_ObjectIndex)
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
void C_SdBueCoAddSignalsModel::m_InitObjectNodeContent(C_TblTreItem & orc_ObjectNode, const uint32 ou32_ObjectIndex)
{
   orc_ObjectNode.c_Name = QString::number(ou32_ObjectIndex, 16);
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
void C_SdBueCoAddSignalsModel::m_InitObjectNodeChildren(C_TblTreItem & orc_ObjectNode, const uint32 ou32_ObjectIndex)
{
   const std::map<stw_types::uint32,
                  std::vector<stw_types::uint32> >::const_iterator c_ItMappableObjects =
      this->mc_MappableObjects.find(ou32_ObjectIndex);
   const std::map<stw_types::uint32,
                  std::vector<C_SdBueCoAddSignalsResultEntry> >::const_iterator c_ItMappableObjectContent =
      this->mc_MappableObjectContent.find(ou32_ObjectIndex);

   tgl_assert(c_ItMappableObjects != this->mc_MappableObjects.end());
   tgl_assert(c_ItMappableObjectContent != this->mc_MappableObjectContent.end());
   if ((c_ItMappableObjects != this->mc_MappableObjects.end()) &&
       (c_ItMappableObjectContent != this->mc_MappableObjectContent.end()))
   {
      tgl_assert(c_ItMappableObjects->second.size() == c_ItMappableObjectContent->second.size());
      if (c_ItMappableObjects->second.size() == c_ItMappableObjectContent->second.size())
      {
         for (uint32 u32_ItEntry = 0UL; u32_ItEntry < c_ItMappableObjects->second.size(); ++u32_ItEntry)
         {
            C_TblTreItem * const pc_SignalItem = new C_TblTreItem();
            m_InitSignalNodeContent(*pc_SignalItem, ou32_ObjectIndex, c_ItMappableObjects->second[u32_ItEntry],
                                    u32_ItEntry);
            orc_ObjectNode.AddChild(pc_SignalItem);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init signal node content

   \param[in,out]  orc_SignalNode         Signal node
   \param[in]      ou32_ObjectIndex       Object index
   \param[in]      ou32_ObjectSubIndex    Object sub index
   \param[in]      ou32_SignalIndex       Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_InitSignalNodeContent(C_TblTreItem & orc_SignalNode, const uint32 ou32_ObjectIndex,
                                                       const uint32 ou32_ObjectSubIndex, const uint32 ou32_SignalIndex)
{
   orc_SignalNode.c_Name = C_SdBueCoAddSignalsModel::mh_GetSignalNodeIndex(ou32_ObjectIndex, ou32_ObjectSubIndex);
   orc_SignalNode.c_Icon = this->mc_IconSignal;

   orc_SignalNode.q_Enabled = true;
   orc_SignalNode.q_Selectable = true;

   orc_SignalNode.u32_Index = ou32_SignalIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get signal node index

   \param[in]  ou32_ObjectIndex     Object index
   \param[in]  ou32_ObjectSubIndex  Object sub index

   \return
   Signal node index
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueCoAddSignalsModel::mh_GetSignalNodeIndex(const uint32 ou32_ObjectIndex, const uint32 ou32_ObjectSubIndex)
{
   const QString c_Retval = static_cast<QString>("%1sub%2").arg(ou32_ObjectIndex, 0, 16).arg(ou32_ObjectSubIndex);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decode index

   \param[in]   orc_ModelIndex      Model index
   \param[out]  oru32_ObjectIndex   Object index
   \param[out]  oru32_SignalIndex   Signal index
   \param[in]   orq_IsSignal        Flag: is signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::mh_DecodeIndex(const QModelIndex & orc_ModelIndex, uint32 & oru32_ObjectIndex,
                                              uint32 & oru32_SignalIndex, bool & orq_IsSignal)
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
   \param[in]      osn_Role            Data role
   \param[in,out]  orc_Output          Output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueCoAddSignalsModel::m_GetData(const E_Columns oe_Column, const uint32 ou32_ObjectIndex,
                                         const uint32 ou32_SignalIndex, const bool oq_IsSignal, const sintn osn_Role,
                                         QVariant & orc_Output) const
{
   if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
   {
      const C_OSCCanOpenObjectDictionary * pc_EdsDictionary;
      if (oq_IsSignal)
      {
         const std::map<stw_types::uint32,
                        std::vector<stw_types::uint32> >::const_iterator c_ItMappableObjects =
            this->mc_MappableObjects.find(ou32_ObjectIndex);
         const std::map<stw_types::uint32,
                        std::vector<C_SdBueCoAddSignalsResultEntry> >::const_iterator c_ItMappableObjectContent =
            this->mc_MappableObjectContent.find(ou32_ObjectIndex);

         tgl_assert(c_ItMappableObjects != this->mc_MappableObjects.end());
         tgl_assert(c_ItMappableObjectContent != this->mc_MappableObjectContent.end());
         if ((c_ItMappableObjects != this->mc_MappableObjects.end()) &&
             (c_ItMappableObjectContent != this->mc_MappableObjectContent.end()))
         {
            tgl_assert(c_ItMappableObjects->second.size() == c_ItMappableObjectContent->second.size());
            tgl_assert(ou32_SignalIndex < c_ItMappableObjectContent->second.size());
            if ((c_ItMappableObjects->second.size() == c_ItMappableObjectContent->second.size()) &&
                (ou32_SignalIndex < c_ItMappableObjectContent->second.size()))
            {
               const C_SdBueCoAddSignalsResultEntry & rc_ResultEntry =
                  c_ItMappableObjectContent->second[ou32_SignalIndex];
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
                     const C_OSCCanOpenObject * const pc_Object = pc_EdsDictionary->GetCanOpenSubIndexObject(
                        ou32_ObjectIndex, static_cast<uint8>(c_ItMappableObjects->second[ou32_SignalIndex]));
                     if (pc_Object != NULL)
                     {
                        orc_Output = C_OSCImportEdsDcf::h_GetObjectName(*pc_Object).c_str();
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
               const C_OSCCanOpenObject * const pc_Object = pc_EdsDictionary->GetCanOpenObject(ou32_ObjectIndex);
               if (pc_Object != NULL)
               {
                  orc_Output = C_OSCImportEdsDcf::h_GetObjectName(*pc_Object).c_str();
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
const C_OSCCanOpenObjectDictionary * C_SdBueCoAddSignalsModel::m_GetEdsDictionary() const
{
   uint8 u8_InterfaceNumber;
   const C_OSCCanOpenObjectDictionary * pc_Retval = NULL;

   if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(this->mc_MessageId.u32_NodeIndex,
                                                                       this->mc_MessageId.u32_InterfaceIndex,
                                                                       u8_InterfaceNumber) == C_NO_ERR)
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
      if (pc_Message != NULL)
      {
         const C_OSCCanOpenManagerDeviceInfo * const pc_Device =
            C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mc_MessageId.u32_NodeIndex,
                                                                     u8_InterfaceNumber,
                                                                     pc_Message->c_CanOpenManagerOwnerNodeIndex);
         if (pc_Device != NULL)
         {
            pc_Retval = &pc_Device->c_EDSFileContent;
         }
      }
   }
   return pc_Retval;
}
