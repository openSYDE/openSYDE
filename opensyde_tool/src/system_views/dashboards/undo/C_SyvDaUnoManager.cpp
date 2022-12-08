//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for dashboard undo command stack (implementation)

   Handler class for dashboard undo command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaUnoManager.hpp"

#include "C_SyvDaUnoAddCommand.hpp"
#include "C_SyvDaUnoAddSnapshotCommand.hpp"
#include "C_SyvDaUnoDeleteCommand.hpp"

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

   \param[in,out] opc_Scene  Scene to use undo redo framework for
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoManager::C_SyvDaUnoManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent) :
   C_SebUnoBaseManager(opc_Scene, opc_Parent)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoManager::~C_SyvDaUnoManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete

   \param[in,out] orc_Items Items to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoManager::DoDelete(const QList<QGraphicsItem *> & orc_Items)
{
   if (orc_Items.size() > 0)
   {
      std::vector<uint64_t> c_Ids;
      C_SyvDaUnoDeleteCommand * pc_DeleteCommand;

      mh_MapItemToId(orc_Items, c_Ids);

      pc_DeleteCommand = new C_SyvDaUnoDeleteCommand(this->mpc_Scene, c_Ids);
      this->DoPush(pc_DeleteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add one element

   \param[in] ore_Type                  Type
   \param[in] oru64_UniqueId            Unique ID
   \param[in] orc_NewPos                Position
   \param[in] of64_ZetValue               Z value
   \param[in] orq_DarkModeDefault       Dark mode flag
   \param[in] orc_AdditionalInformation Additional string information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoManager::DoAddGeneric(const C_PuiSvDbDataElement::E_Type & ore_Type, const uint64_t & oru64_UniqueId,
                                     const QPointF & orc_NewPos, const float64_t of64_ZetValue,
                                     const bool & orq_DarkModeDefault, const QString & orc_AdditionalInformation)
{
   std::vector<uint64_t> c_Ids;
   C_SyvDaUnoAddCommand * pc_AddCommand;

   c_Ids.push_back(oru64_UniqueId);
   pc_AddCommand = new C_SyvDaUnoAddCommand(this->mpc_Scene, c_Ids, ore_Type, orc_NewPos, of64_ZetValue,
                                            orc_AdditionalInformation,
                                            NULL, false, orq_DarkModeDefault);
   this->DoPush(pc_AddCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new data based on a snapshot and reserved IDs

   \param[in] orc_UniqueIds        Reserved unique IDs for snapshot data
   \param[in] orc_Snapshot           Snapshot data
   \param[in] orc_RestoredRails      Rails to restore if possible
   \param[in] orc_NewPos             New position
   \param[in] of64_HighestUsedZetValue Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoManager::DoAddSnapshot(const std::vector<uint64_t> & orc_UniqueIds,
                                      const C_SyvDaDashboardSnapshot & orc_Snapshot,
                                      const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                 C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                      const QPointF & orc_NewPos,
                                      const float64_t of64_HighestUsedZetValue)
{
   C_SyvDaUnoAddSnapshotCommand * pc_AddCommand;

   pc_AddCommand = new C_SyvDaUnoAddSnapshotCommand(this->mpc_Scene, orc_Snapshot, orc_UniqueIds, orc_RestoredRails,
                                                    orc_NewPos, of64_HighestUsedZetValue);
   this->DoPush(pc_AddCommand);
}
