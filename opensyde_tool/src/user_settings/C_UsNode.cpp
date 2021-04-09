//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node user settings (implementation)

   Project node user settings

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_UsNode.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNode::C_UsNode(void) :
   me_SelectedProtocol(stw_opensyde_core::C_OSCCanProtocol::eLAYER2),
   mu32_SelectedInterface(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition node datapool user settings

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)

   \return
   Project system definition node datapool user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNodeDatapool C_UsNode::GetDatapool(const QString & orc_DatapoolName) const
{
   return this->mc_Datapools.value(orc_DatapoolName, C_UsNodeDatapool());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get datapool keys internal structure

   \return
   Datapool keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsNode::GetDatapoolKeysInternal(void) const
{
   return this->mc_Datapools.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data pool name

   \return
   Selected data pool name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedDatapoolName(void) const
{
   return this->mc_SelectedDataPoolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected protocol type

   \return
   Selected protocol type
*/
//----------------------------------------------------------------------------------------------------------------------
stw_opensyde_core::C_OSCCanProtocol::E_Type C_UsNode::GetSelectedProtocol(void) const
{
   return this->me_SelectedProtocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected CAN interface index

   \return
   Selected CAN interface index
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_UsNode::GetSelectedInterface(void) const
{
   return this->mu32_SelectedInterface;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get halc overview column width

   \return
   Halc overview column width
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<stw_types::sint32> & C_UsNode::GetHalcOverviewColumnWidth(void) const
{
   return this->mc_HalcOverviewColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC parameter configuration table column widths

   \return
   widths of parameter configuration table columns
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<stw_types::sint32> & C_UsNode::GetHalcConfigColumnWidth() const
{
   return this->mc_HalcConfigColumnWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected HALC domain name resp. domain name of selected HALC channel

   \return
   selected HALC domain name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedHalcDomainName(void) const
{
   return this->mc_SelectedHalcDomainName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected HALC channel ID

   \return
   Selected channel ID
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsNode::GetSelectedHalcChannel(void) const
{
   return this->mc_SelectedHalcChannelId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selected data pool name

   \param[in]  orc_DatapoolName  Selected data pool name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedDatapoolName(const QString & orc_DatapoolName)
{
   this->mc_SelectedDataPoolName = orc_DatapoolName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set selected protocol

   \param[in]  oe_Protocol    Selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol)
{
   this->me_SelectedProtocol = oe_Protocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected CAN interface index

   \param[in]  ou32_InterfaceIndex    Selected CAN interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedInterface(const stw_types::uint32 ou32_InterfaceIndex)
{
   this->mu32_SelectedInterface = ou32_InterfaceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool expanded list names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Expanded list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetExpandedListNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetExpandedListNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected list names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetSelectedListNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetSelectedListNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected variable names

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetSelectedVariableNames(orc_New);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetSelectedVariableNames(orc_New);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set datapool comm overview column width

   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolCommMessageOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                         const std::vector<stw_types::sint32> & orc_Value)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetCommMessageOverviewColumnWidth(orc_ListName, orc_Value);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetCommMessageOverviewColumnWidth(orc_ListName, orc_Value);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set datapool comm signal overview column width

   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolCommSignalOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                        const std::vector<stw_types::sint32> & orc_Value)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetCommSignalOverviewColumnWidth(orc_ListName, orc_Value);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetCommSignalOverviewColumnWidth(orc_ListName, orc_Value);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set datapool list selected message

   \param[in]  orc_DatapoolName           Datapool name (identifier)
   \param[in]  orc_ListName               Datapool list name (identifier)
   \param[in]  oe_SelectedProtocol        Protocol type
   \param[in]  oq_MessageSelected         Set flag if there is a selected message
   \param[in]  orc_SelectedMessageName    Selected message name if any
   \param[in]  oq_SignalSelected          Flag if signal selected
   \param[in]  orc_SelectedSignalName     Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                              const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                              const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                              const bool oq_SignalSelected, const QString & orc_SelectedSignalName)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetListSelectedMessage(orc_ListName, oe_SelectedProtocol,
                                                                              oq_MessageSelected,
                                                                              orc_SelectedMessageName,
                                                                              oq_SignalSelected,
                                                                              orc_SelectedSignalName);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetListSelectedMessage(orc_ListName, oe_SelectedProtocol, oq_MessageSelected, orc_SelectedMessageName,
                                        oq_SignalSelected, orc_SelectedSignalName);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set datapool list column widths

   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_ListName      Project system definition node datapool list name (identifier)
   \param[in]  orc_ColumnWidths  Last known column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                          const std::vector<stw_types::sint32> & orc_ColumnWidths)
{
   if (this->mc_Datapools.contains(orc_DatapoolName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_Datapools.operator [](orc_DatapoolName).SetListColumnSizes(orc_ListName, orc_ColumnWidths);
   }
   else
   {
      C_UsNodeDatapool c_Datapool;
      c_Datapool.SetListColumnSizes(orc_ListName, orc_ColumnWidths);
      this->mc_Datapools.insert(orc_DatapoolName, c_Datapool);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set halc overview column width

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetHalcOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value)
{
   this->mc_HalcOverviewColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set HALC parameter configuration table column widths

   \param[in]  orc_Value   Column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetHalcConfigColumnWidth(const std::vector<stw_types::sint32> & orc_Value)
{
   this->mc_HalcConfigColumnWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected HALC domain name

   \param[in]  orc_Value   Domain name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedHalcDomain(const QString & orc_Value)
{
   this->mc_SelectedHalcDomainName = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set selected HALC channel ID

   \param[in]  orc_Value   Channel ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNode::SetSelectedHalcChannel(const QString & orc_Value)
{
   this->mc_SelectedHalcChannelId = orc_Value;
}
