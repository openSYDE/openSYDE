//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node datapool user settings (implementation)

   Project node datapool user settings

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_UsNodeDatapool.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

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
C_UsNodeDatapool::C_UsNodeDatapool(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system definition node datapool expanded list names

   \return
   Current system definition node datapool expanded list names
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<QString> & C_UsNodeDatapool::GetExpandedListNames(void) const
{
   return this->mc_ExpandedListNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set system definition node datapool expanded list names

   \param[in]  orc_Value   New system definition node datapool expanded list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetExpandedListNames(const std::vector<QString> & orc_Value)
{
   this->mc_ExpandedListNames = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system definition node datapool selected list names

   \return
   Current system definition node datapool selected list names
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<QString> & C_UsNodeDatapool::GetSelectedListNames() const
{
   return this->mc_SelectedListNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set system definition node datapool selected list names

   \param[in]  orc_Value   New system definition node datapool selected list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetSelectedListNames(const std::vector<QString> & orc_Value)
{
   this->mc_SelectedListNames = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system definition node datapool selected variable names

   \return
   Current system definition node datapool selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<QString> & C_UsNodeDatapool::GetSelectedVariableNames(void) const
{
   return this->mc_SelectedVariableNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set system definition node datapool selected variable names

   \param[in]  orc_Value   New system definition node datapool selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetSelectedVariableNames(const std::vector<QString> & orc_Value)
{
   this->mc_SelectedVariableNames = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interface settings keys internal structure

   \return
   Interface settings keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsNodeDatapool::GetInterfaceSettingsKeysInternal(void) const
{
   return this->mc_InterfaceSettings.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get list user settings

   \param[in]  orc_ListName   List name (identifier)

   \return
   List user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsCommunication C_UsNodeDatapool::GetCommList(const QString & orc_ListName) const
{
   return this->mc_InterfaceSettings.value(orc_ListName, C_UsCommunication());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set comm overview column width

   \param[in]  orc_ListName   List name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetCommMessageOverviewColumnWidth(const QString & orc_ListName,
                                                         const std::vector<int32_t> & orc_Value)
{
   if (this->mc_InterfaceSettings.contains(orc_ListName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_InterfaceSettings.operator [](orc_ListName).SetMessageOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetMessageOverviewColumnWidth(orc_Value);
      this->mc_InterfaceSettings.insert(orc_ListName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set comm signal overview column width

   \param[in]  orc_ListName   List name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetCommSignalOverviewColumnWidth(const QString & orc_ListName,
                                                        const std::vector<int32_t> & orc_Value)
{
   if (this->mc_InterfaceSettings.contains(orc_ListName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_InterfaceSettings.operator [](orc_ListName).SetSignalOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetSignalOverviewColumnWidth(orc_Value);
      this->mc_InterfaceSettings.insert(orc_ListName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list last selected message

   \param[in]  orc_ListName               List name (identifier)
   \param[in]  oe_SelectedProtocol        Currently selected protocol
   \param[in]  oq_MessageSelected         Set flag if there is a selected message
   \param[in]  orc_SelectedMessageName    Selected message name if any
   \param[in]  oq_SignalSelected          Flag if signal selected
   \param[in]  orc_SelectedSignalName     Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetListSelectedMessage(const QString & orc_ListName,
                                              const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                              const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                              const bool oq_SignalSelected, const QString & orc_SelectedSignalName)
{
   if (this->mc_InterfaceSettings.contains(orc_ListName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_InterfaceSettings.operator [](orc_ListName).SetMessageSelected(oe_SelectedProtocol, oq_MessageSelected,
                                                                              orc_SelectedMessageName,
                                                                              oq_SignalSelected,
                                                                              orc_SelectedSignalName);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetMessageSelected(oe_SelectedProtocol, oq_MessageSelected, orc_SelectedMessageName, oq_SignalSelected,
                               orc_SelectedSignalName);
      this->mc_InterfaceSettings.insert(orc_ListName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get list settings keys internal structure

   \return
   List settings keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsNodeDatapool::GetListSettingsKeysInternal() const
{
   return this->mc_ListSettings.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get list user settings

   \param[in]  orc_ListName   List name (identifier)

   \return
   List user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNodeDatapoolList C_UsNodeDatapool::GetOtherList(const QString & orc_ListName) const
{
   return this->mc_ListSettings.value(orc_ListName, C_UsNodeDatapoolList());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set list column widths

   \param[in]  orc_ListName      Project system definition node datapool list name (identifier)
   \param[in]  orc_ColumnWidths  Last known column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsNodeDatapool::SetListColumnSizes(const QString & orc_ListName, const std::vector<int32_t> & orc_ColumnWidths)
{
   if (this->mc_ListSettings.contains(orc_ListName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ListSettings.operator [](orc_ListName).SetColumnSizes(orc_ColumnWidths);
   }
   else
   {
      C_UsNodeDatapoolList c_List;
      c_List.SetColumnSizes(orc_ColumnWidths);
      this->mc_ListSettings.insert(orc_ListName, c_List);
   }
}
