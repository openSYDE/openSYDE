//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Project node user settings (implementation)

   Project node user settings

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_UsNode.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsNode::C_UsNode(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get project system definition node datapool user settings

   \param[in] orc_ViewName Project system definition node datapool name (identifier)

   \return
   Project system definition node datapool user settings

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsNodeDatapool C_UsNode::GetDatapool(const QString & orc_DatapoolName) const
{
   return this->mc_Datapools.value(orc_DatapoolName, C_UsNodeDatapool());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get datapool keys internal structure

   \return
   Datapool keys internal structure

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const QList<QString> C_UsNode::GetDatapoolKeysInternal(void) const
{
   return this->mc_Datapools.keys();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get selected data pool name

   \return
   Selected data pool name

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_UsNode::GetSelectedDatapoolName(void) const
{
   return this->mc_SelectedDataPoolName;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set selected data pool name

   \param[in] orc_DatapoolName Selected data pool name

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsNode::SetSelectedDatapoolName(const QString & orc_DatapoolName)
{
   this->mc_SelectedDataPoolName = orc_DatapoolName;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set project system definition node datapool expanded list names

   \param[in] orc_DatapoolName Project system definition node datapool name (identifier)
   \param[in] orc_New          Expanded list names

   \created     23.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set project system definition node datapool selected list names

   \param[in] orc_DatapoolName Project system definition node datapool name (identifier)
   \param[in] orc_New          Selected list names

   \created     24.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set project system definition node datapool selected variable names

   \param[in] orc_DatapoolName Project system definition node datapool name (identifier)
   \param[in] orc_New          Selected variable names

   \created     25.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set datapool list selected message

   \param[in] orc_DatapoolName        Datapool name (identifier)
   \param[in] orc_ListName            Datapool list name (identifier)
   \param[in] oq_MessageSelected      Set flag if there is a selected message
   \param[in] orc_SelectedMessageName Selected message name if any
   \param[in] oq_SignalSelected       Flag if signal selected
   \param[in] orc_SelectedSignalName  Selected signal name if any

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set datapool list column widths

   \param[in] orc_DatapoolName Project system definition node datapool name (identifier)
   \param[in] orc_ListName     Project system definition node datapool list name (identifier)
   \param[in] orc_ColumnWidths Last known column widths

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
