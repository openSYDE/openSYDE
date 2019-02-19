//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring database files like DBC or syde_sysdef files.
 (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASEWIDGET_H
#define C_CAMMOSDATABASEWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeWiOnlyBackground.h"
#include "C_CamMosDatabaseItemWidget.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_CamMosDatabaseWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMosDatabaseWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosDatabaseWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosDatabaseWidget(void);

   void LoadUserSettings(void) const;
   void OnLoadFinishedDbc(const stw_types::sint32 os32_Result);
   void OnLoadFinishedOsySysDef(const stw_types::sint32 os32_Result,
                                const std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses);
   void OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition, const stw_types::sint32 os32_Result);
   void OnCommunicationStarted(const bool oq_Online);
   void PrepareForExpanded(const bool oq_Expand) const;
   void OnSigSavedAsNew(void) const;
   void AddDroppedDatabase(const QString & orc_Path);
   void Clear(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigAddDatabaseDbc(const QString & orc_Path);
   void SigAddDatabaseOsySysDef(const QString & orc_Path, const stw_types::uint32 ou32_BusIndex);
   void SigRemoveDatabase(const QString & orc_Path, const bool oq_IsUpdate);
   void SigActivateDatabase(const QString & orc_Path, const bool oq_Active);
   void SigSetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition, const stw_types::uint32 ou32_BusIndex);

private:
   Ui::C_CamMosDatabaseWidget * mpc_Ui;
   std::vector<C_CamMosDatabaseItemWidget *> mc_Entries;
   QList<C_CamMosDatabaseItemWidget *> mc_DatabasesToLoad;
   QTimer mc_Timer;

   //Avoid call
   C_CamMosDatabaseWidget(const C_CamMosDatabaseWidget &);
   C_CamMosDatabaseWidget & operator =(const C_CamMosDatabaseWidget &);

   void m_InitUi(void);
   void m_LoadConfig(void);
   void m_RemoveDatabaseWidget(C_CamMosDatabaseItemWidget * const opc_ItemWidget) const;
   void m_OnAddClicked(void);
   void m_AddDatabaseWidget(const stw_opensyde_gui_logic::C_CamProDatabaseData & orc_DatabaseData);
   void m_RemoveDatabase(C_CamMosDatabaseItemWidget * const opc_ItemWidget, const bool oq_AskUser);
   void m_ActivateDatabase(const C_CamMosDatabaseItemWidget * const opc_ItemWidget, const bool & orq_Enable);
   void m_UpdateDatabasePath(C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                             const stw_opensyde_gui_logic::C_CamProDatabaseData & orc_Database, const bool oq_IsUpdate);
   void m_UpdateDatabaseOsySysDefBus(const C_CamMosDatabaseItemWidget * const opc_ItemWidget,
                                     const stw_types::sint32 os32_BusIndex);
   stw_types::sint32 m_GetIndexFromWidget(const C_CamMosDatabaseItemWidget * const opc_ItemWidget);
   void m_AddToLoadingQueue(C_CamMosDatabaseItemWidget * const opc_ItemWidget);
   void m_CheckAndLoadDatabase(void);
   void m_LoadFinishedGeneric(const stw_types::sint32 os32_ErrorCode);
   void m_OnFileCheckTimeout(void);
   void m_StartCheckTimer(const bool oq_Active);
   void m_OnExpand(const bool oq_Expand) const;
   void m_UpdateTitleDatabaseCount(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
