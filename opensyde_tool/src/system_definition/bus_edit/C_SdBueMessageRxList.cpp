//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Message receiver edit list (implementation)

   Message receiver edit list

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_SdBueMessageRxList.h"
#include "ui_C_SdBueMessageRxList.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueMessageRxList::C_SdBueMessageRxList(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageRxList),
   mu32_LastKnownCycleTimeValue(0),
   mq_AlwaysHide(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ScrollAreaWidget->SetBackgroundColor(-1);

   InitStaticNames();

   //Debug strings
   this->mpc_Ui->pc_GroupBoxEmpty->setTitle("");
   this->mpc_Ui->pc_GroupBoxNotEmpty->setTitle("");

   this->mpc_Ui->pc_ScrollArea->DeactivateScrollbarResize();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueMessageRxList::~C_SdBueMessageRxList(void)
{
   m_Clear();
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     28.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelEmpty->setText(C_GtGetText::h_GetText("No receivers available."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add all entries

   \param[in] orc_EntryNames                 Entry name
   \param[in] orc_NodeIndexes                Node index (ID)
   \param[in] orc_InterfaceIndexes           Interface Index (ID)
   \param[in] orc_UseAutoReceiveTimeoutFlags Flag whether to use auto receive timeout or custom
   \param[in] orc_ReceiveTimeoutValues       Receive timeout value

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::AddNodes(const std::vector<QString> & orc_EntryNames,
                                    const std::vector<uint32> & orc_NodeIndexes,
                                    const std::vector<uint32> & orc_InterfaceIndexes,
                                    const std::vector<bool> & orc_UseAutoReceiveTimeoutFlags,
                                    const std::vector<uint32> & orc_ReceiveTimeoutValues)
{
   m_Clear();
   //Can happen at start up
   if ((((orc_EntryNames.size() == orc_NodeIndexes.size()) && (orc_EntryNames.size() == orc_InterfaceIndexes.size())) &&
        (orc_EntryNames.size() == orc_UseAutoReceiveTimeoutFlags.size())) &&
       (orc_EntryNames.size() == orc_ReceiveTimeoutValues.size()))
   {
      this->mc_Entries.reserve(orc_EntryNames.size());
      for (uint32 u32_ItEntry = 0; u32_ItEntry < orc_EntryNames.size(); ++u32_ItEntry)
      {
         C_SdBueMessageRxEntry * const pc_Entry = new C_SdBueMessageRxEntry(this);
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeToggled, this, &C_SdBueMessageRxList::SigNodeToggled);
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeReceiveTimeout, this,
                 &C_SdBueMessageRxList::SigNodeReceiveTimeout);
         connect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeUseAutoReceiveTimeout, this,
                 &C_SdBueMessageRxList::SigNodeUseReceiveTimeout);
         pc_Entry->Init(orc_EntryNames[u32_ItEntry], orc_NodeIndexes[u32_ItEntry], orc_InterfaceIndexes[u32_ItEntry],
                        orc_UseAutoReceiveTimeoutFlags[u32_ItEntry], orc_ReceiveTimeoutValues[u32_ItEntry]);
         pc_Entry->SetLastKnownCycleTimeValue(this->mu32_LastKnownCycleTimeValue);
         pc_Entry->SetAlwaysHideTimeout(this->mq_AlwaysHide);
         this->mpc_Ui->pc_VerticalLayout->insertWidget(this->mpc_Ui->pc_VerticalLayout->count() - 1, pc_Entry);
         this->mc_Entries.push_back(pc_Entry);
      }
      //Visibility
      if (orc_EntryNames.size() == 0UL)
      {
         this->mpc_Ui->pc_GroupBoxEmpty->setVisible(true);
         this->mpc_Ui->pc_GroupBoxNotEmpty->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_GroupBoxEmpty->setVisible(false);
         this->mpc_Ui->pc_GroupBoxNotEmpty->setVisible(true);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update last known cycle time value

   \param[in] ou32_Value Last known cycle time value

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::SetLastKnownCycleTimeValue(const uint32 ou32_Value)
{
   this->mu32_LastKnownCycleTimeValue = ou32_Value;
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         pc_Entry->SetLastKnownCycleTimeValue(ou32_Value);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set flag to always hide timeout section

   \param[in] oq_Hide Flag to always hide timeout section

   \created     23.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::SetAlwaysHideTimeout(const bool oq_Hide)
{
   this->mq_AlwaysHide = oq_Hide;
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_ItEntry];
      if (pc_Entry != NULL)
      {
         pc_Entry->SetAlwaysHideTimeout(oq_Hide);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check specific entries

   \param[in] orc_NodeIndexes      Node index (ID)
   \param[in] orc_InterfaceIndexes Interface Index (ID)

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::CheckNodes(const std::vector<uint32> & orc_NodeIndexes,
                                      const std::vector<uint32> & orc_InterfaceIndexes) const
{
   if (orc_NodeIndexes.size() == orc_InterfaceIndexes.size())
   {
      for (uint32 u32_ItNode = 0; u32_ItNode < orc_NodeIndexes.size(); ++u32_ItNode)
      {
         this->CheckSpecificNode(orc_NodeIndexes[u32_ItNode], orc_InterfaceIndexes[u32_ItNode], true);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check specific entry

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface Index (ID)
   \param[in] oq_Checked          Status

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::CheckSpecificNode(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                             const bool oq_Checked) const
{
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_ItEntry];
      if ((pc_Entry != NULL) && (pc_Entry->DoesMatch(ou32_NodeIndex, ou32_InterfaceIndex) == true))
      {
         pc_Entry->SetChecked(oq_Checked);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMessageRxList::m_Clear(void)
{
   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      C_SdBueMessageRxEntry * const pc_Entry = this->mc_Entries[u32_ItEntry];
      disconnect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeToggled, this, &C_SdBueMessageRxList::SigNodeToggled);
      disconnect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeReceiveTimeout, this,
                 &C_SdBueMessageRxList::SigNodeReceiveTimeout);
      disconnect(pc_Entry, &C_SdBueMessageRxEntry::SigNodeUseAutoReceiveTimeout, this,
                 &C_SdBueMessageRxList::SigNodeUseReceiveTimeout);
      this->mpc_Ui->pc_VerticalLayout->removeWidget(pc_Entry);
      pc_Entry->deleteLater();
   }
   this->mc_Entries.clear();
}
