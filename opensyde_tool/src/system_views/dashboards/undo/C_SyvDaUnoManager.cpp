//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Handler class for dashboard undo command stack (implementation)

   Handler class for dashboard undo command stack

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaUnoManager.h"

#include "C_SyvDaUnoAddCommand.h"
#include "C_SyvDaUnoAddSnapshotCommand.h"
#include "C_SyvDaUnoDeleteCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   \param[in,out] opc_Scene  Scene to use undo redo framework for
   \param[in,out] opc_Parent Optional pointer to parent

   \created     21.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoManager::C_SyvDaUnoManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent) :
   C_SebUnoBaseManager(opc_Scene, opc_Parent)
{
}
//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     21.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaUnoManager::~C_SyvDaUnoManager(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do delete

   \param[in,out] orc_Items Items to delete

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoManager::DoDelete(const QList<QGraphicsItem *> & orc_Items)
{
   if (orc_Items.size() > 0)
   {
      std::vector<uint64> c_IDs;
      C_SyvDaUnoDeleteCommand * pc_DeleteCommand;

      m_MapItemToID(orc_Items, c_IDs);

      pc_DeleteCommand = new C_SyvDaUnoDeleteCommand(this->mpc_Scene, c_IDs);
      this->DoPush(pc_DeleteCommand);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add one element

   \param[in] ore_Type                  Type
   \param[in] oru64_UniqueID            Unique ID
   \param[in] orc_NewPos                Position
   \param[in] orc_AdditionalInformation Additional string information

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoManager::DoAddGeneric(const C_PuiSvDbDataElement::E_Type & ore_Type, const uint64 & oru64_UniqueID,
                                     const QPointF & orc_NewPos, const bool & orq_DarkModeDefault,
                                     const QString & orc_AdditionalInformation)
{
   std::vector<uint64> c_IDs;
   C_SyvDaUnoAddCommand * pc_AddCommand;

   c_IDs.push_back(oru64_UniqueID);
   pc_AddCommand = new C_SyvDaUnoAddCommand(this->mpc_Scene, c_IDs, ore_Type, orc_NewPos, orc_AdditionalInformation,
                                            NULL, false, orq_DarkModeDefault);
   this->DoPush(pc_AddCommand);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new data based on a snapshot and reserved IDs

   \param[in] oru64_UniqueIDs   Reserved unique IDs for snapshot data
   \param[in] orc_Snapshot      Snapshot data
   \param[in] orc_RestoredRails Rails to restore if possible
   \param[in] orc_NewPos        New position

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaUnoManager::DoAddSnapshot(const std::vector<uint64> & oru64_UniqueIDs,
                                      const C_PuiSvDashboard & orc_Snapshot,
                                      const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                      const QPointF & orc_NewPos)
{
   C_SyvDaUnoAddSnapshotCommand * pc_AddCommand;

   pc_AddCommand = new C_SyvDaUnoAddSnapshotCommand(this->mpc_Scene, orc_Snapshot, oru64_UniqueIDs, orc_RestoredRails,
                                                    orc_NewPos);
   this->DoPush(pc_AddCommand);
}
